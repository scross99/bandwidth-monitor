Bandwidth Monitor
-----------------

This software is a combination of a linux kernel module and two userspace programs
which monitor all bandwidth going in/out of the host machine, identify traffic that
is not local and apply bandwidth rate limiting to ensure it can't go over a particular
limit.

The kernel module needs to be installed and loaded at boot time (instructions are
provided for this), and then it can be managed by either the command line program or
GUI program (which creates a system tray icon) provided.

The definition of 'local' clearly depends on the use of the software and needs to be
adjusted appropriately in the kernel module. It's currently set up for the Cambridge
University network.

Also note that this software only limits IPv4 packets; IPv6 is unaffected (although
if you're tunneling to support IPv6 this won't be a problem, since you're still
sending/receiving IPv4 packets).

As for licensing, the kernel module is under the GPL and the userspace programs are
under the MIT license. Both are included in their respective directories.

