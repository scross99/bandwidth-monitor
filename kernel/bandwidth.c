#include <linux/init.h>
#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>

#undef __KERNEL__
#include <linux/netfilter_ipv4.h>
#define __KERNEL__

#include <linux/skbuff.h>
#include <linux/time.h>

MODULE_AUTHOR("Stephen Cross");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Monitors network bandwidth use and dynamically rate-limits as directed by user-space processes.");

static struct nf_hook_ops netfilter_ops_down;
static struct nf_hook_ops netfilter_ops_up;

typedef unsigned long long ULong;

// The number of bytes uploaded/downloaded while the module has been running.
static ULong bytesDown, bytesUp;

// The number of bytes uploaded/downloaded this second.
static ULong secondBytesDown, secondBytesUp;

// The current second value.
static ULong currentSecond;

// The bandwidth limit for this second.
static ULong secondLimit;

// The bandwidth limit for the next second.
static ULong nextSecondLimit;

// How many times the next second bandwidth limit has been set.
static ULong nextSecondSet;

void update(void){
	const ULong thisSecond = get_seconds();
	if(thisSecond != currentSecond){
		// New second -> update/reset values.
		currentSecond = thisSecond;

		secondLimit = nextSecondLimit;
		nextSecondLimit = nextSecondSet = 0ul;
		
		bytesUp += secondBytesUp;
		bytesDown += secondBytesDown;
		
		secondBytesUp = secondBytesDown = 0ul;
	}
}

inline int check(unsigned char v, unsigned char targetValue, unsigned char numBits){
	return (v >> (8 - numBits)) == (targetValue >> (8 - numBits));
}

// Works out whether an address is considered local and hence doesn't
// contribute to bandwidth use. Change this to suit your own purposes.
int isLocal(__be32 addr){
	const unsigned char a = ((unsigned char *) &addr)[0], b = ((unsigned char *) &addr)[1], c = ((unsigned char *) &addr)[2], d = ((unsigned char *) &addr)[3];
	switch(a){
		case 127:
			/*
			127.0.0.0/8 - Loopback
			*/
			return 1;
		case 128:
			/*
			128.232.0.0/16
			*/
			return b == 232;
		case 129:
			/*
			129.169.0.0/16
			*/
			return b == 169;
		case 131:
			/*
			131.111.0.0/16
			*/
			return b == 111;
		case 169:
			/*
			169.254.0.0/16 - Link-Local
			*/
			return b == 254;
		case 172:
			/*
			172.16.0.0/13
			172.24.0.0/14
			172.28.0.0/15
			172.30.0.0/16
			*/
			return check(b, 16, 5) || check(b, 24, 6) || check(b, 28, 7) || b == 30;
		case 192:
			/*
			192.18.195.0/24 - Medical Research Council
			192.84.5.0/24
			192.153.213.0/24
			192.168.0.0/16 - Private network
			*/
			switch(b){
				case 18:
					return c == 195;
				case 84:
					return c == 5;
				case 153:
					return c == 213;
				case 168:
					return 1;
				default:
					return 0;
			}
		case 193:
			/*
			193.60.80.0/20
			193.63.252.0/23 - Medical Research Council
			*/
			switch(b){
				case 60:
					return check(c, 80, 4);
				case 63:
					return check(c, 252, 7);
				default:
					return 0;
			}
		case 255:
			/*
			255.255.255.255 - Broadcast
			*/
			return b == 255 && c == 255 && d == 255;
		default:
			return 0;
	}
}

// Download Traffic.
unsigned int down_hook(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*)){
	struct iphdr * ipHeader;
	__be32 sourceIP;
	ULong packetSize;
	
	ipHeader = ip_hdr(skb);

	// If there isn't IP header it's unlikely to be a problem.
	if(!ipHeader){ return NF_ACCEPT; }

	sourceIP = ipHeader->saddr;

	if(isLocal(sourceIP)){
		// All packets from local addresses are ignored.
		return NF_ACCEPT;
	}

	packetSize = ntohs(ipHeader->tot_len);
	
	update();
	
	// Even if incoming packets are to be dropped, they still have to be added to the usage.
	secondBytesDown += packetSize;

	return ((secondBytesUp + secondBytesDown + packetSize) <= secondLimit) ? NF_ACCEPT : NF_DROP;
}

// Upload Traffic.
unsigned int up_hook(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*)){
	struct iphdr * ipHeader;
	__be32 destIP;
	ULong packetSize;
	
	ipHeader = ip_hdr(skb);

	// If there isn't IP header it's unlikely to be a problem.
	if(!ipHeader){ return NF_ACCEPT; }

	destIP = ipHeader->daddr;

	if(isLocal(destIP)){
		// All packets to local addresses are ignored.
		return NF_ACCEPT;
	}
	
	packetSize = ntohs(ipHeader->tot_len);
	
	update();
	
	if(((secondBytesUp + secondBytesDown + packetSize) <= secondLimit)){
		secondBytesUp += packetSize;
		return NF_ACCEPT;
	}else{
		// Dropped outgoing packets don't need to be added to the total.
		return NF_DROP;
	}
}

// Functions called when files in module's /sys/... directory are accessed.
static ssize_t up_show(struct kobject * kobj, struct kobj_attribute * attr, char * buf){
	update();
	return sprintf(buf, "%llu", bytesUp);
}

static ssize_t down_show(struct kobject * kobj, struct kobj_attribute * attr, char * buf){
	update();
	return sprintf(buf, "%llu", bytesDown);
}

static ssize_t seconds_show(struct kobject * kobj, struct kobj_attribute * attr, char * buf){
	update();
	return sprintf(buf, "%llu", currentSecond);
}

static ssize_t limit_show(struct kobject * kobj, struct kobj_attribute * attr, char * buf){
	update();
	return sprintf(buf, "%llu", secondLimit);
}

static ssize_t limit_store(struct kobject * kobj, struct kobj_attribute * attr, const char * buf, size_t count){
	ULong setSecondLimit = 0;

	update();
	
	sscanf(buf, "%lluu", &setSecondLimit);
	
	// If two or more userspace processes are limiting bandwidth, use the smallest value.
	if(nextSecondSet == 0 || setSecondLimit <= nextSecondLimit){
		nextSecondLimit = setSecondLimit;
	}
	
	nextSecondSet++;
	return count;
}

static struct kobj_attribute up_attribute = __ATTR(up, 0444, up_show, NULL);
static struct kobj_attribute down_attribute = __ATTR(down, 0444, down_show, NULL);
static struct kobj_attribute seconds_attribute = __ATTR(seconds, 0444, seconds_show, NULL);
static struct kobj_attribute limit_attribute = __ATTR(limit, 0666, limit_show, limit_store);

static struct attribute * attrs [] =
{
	&up_attribute.attr,
	&down_attribute.attr,
	&seconds_attribute.attr,
	&limit_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject * kobj;

int init_module(){
        int retval;
        
        netfilter_ops_down.hook                   =       down_hook;
        netfilter_ops_down.pf                     =       PF_INET;
        netfilter_ops_down.hooknum                =       NF_IP_PRE_ROUTING;
        netfilter_ops_down.priority               =       NF_IP_PRI_FIRST;

        netfilter_ops_up.hook                     =       up_hook;
        netfilter_ops_up.pf                       =       PF_INET;
        netfilter_ops_up.hooknum                  =       NF_IP_POST_ROUTING;
        netfilter_ops_up.priority                 =       NF_IP_PRI_FIRST;

        nf_register_hook(&netfilter_ops_down);
        nf_register_hook(&netfilter_ops_up);

	// Initial limits are 0; a userspace program needs to be run to manage this module.
	bytesDown = bytesUp = 0ul;
	secondBytesDown = secondBytesUp = 0ul;
	currentSecond = secondLimit = 0ul;
	nextSecondLimit = nextSecondSet = 0ul;
	
	kobj = kobject_create_and_add("bandwidth", kernel_kobj);
	if(!kobj)
		return ENOMEM;

	retval = sysfs_create_group(kobj, &attr_group);
	if(retval)
		kobject_put(kobj);
	return retval;
}

void cleanup_module(){
	nf_unregister_hook(&netfilter_ops_down);
	nf_unregister_hook(&netfilter_ops_up);

	kobject_put(kobj);
}








