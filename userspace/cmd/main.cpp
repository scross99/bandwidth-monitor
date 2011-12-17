#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <common/Bandwidth.hpp>
#include <common/BandwidthFile.hpp>
#include <common/Display.hpp>
#include <common/Module.hpp>
#include <common/ULong.hpp>

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Format: bandwidth_monitor_cmd [command] [...]" << std::endl;
		std::cout << "    where command is one of the following:" << std::endl;
		std::cout << "    'day-limit' - Enable remote internet traffic with a given total limit for the day" << std::endl;
		std::cout << "    'rate-limit' - Enable remote internet traffic with a given rate limit" << std::endl;
		std::cout << "    'get' - Get current bandwidth consumed" << std::endl;
		std::cout << "    'poll' - Poll current bandwidth consumed" << std::endl;
		return 1;
	}
	
	if(strcmp(argv[1], "day-limit") == 0) {
		if(argc != 3) {
			std::cout << "Format: bandwidth_monitor_cmd day-limit [limit (in bytes)]" << std::endl;
			return 1;
		}
		
		Module module;
		
		BandwidthFile bandwidthFile;
		
		Bandwidth startBandwidth, moduleStartBandwidth;
		
		bandwidthFile.load(&startBandwidth);
		moduleStartBandwidth = module.getBandwidth();
		
		ULong currentSeconds = 0;
		
		const ULong limit = ULong(atof(argv[2]));
		
		std::cout << "Running with day limit of " << displayDataSize(limit) << "..." << std::endl;
		
		const time_t currentTime = time(0);
		const tm * timeData = localtime(&currentTime);
		int currentWeekDay = timeData->tm_wday;
		
		while(true){
			Module module;

			const ULong seconds = module.getSeconds();
			
			if(seconds != currentSeconds){
				currentSeconds = seconds;
				
				const Bandwidth moduleBandwidth = module.getBandwidth();
				
				const time_t currentTime = time(0);
				const tm * timeData = localtime(&currentTime);
				const int weekDay = timeData->tm_wday;
				
				if(weekDay != currentWeekDay){
					currentWeekDay = weekDay;
					moduleStartBandwidth = moduleBandwidth;
					startBandwidth = Bandwidth(0ul, 0ul);
				}
	
				const Bandwidth bandwidth = (moduleBandwidth - moduleStartBandwidth) + startBandwidth;
				
				const ULong total = bandwidth.down + bandwidth.up;
				const ULong maxTransferRate = (limit > total ? limit - total : 0ul) / 60ul;
				
				bandwidthFile.save(bandwidth);
				
				module.setLimit(maxTransferRate);
			}
			
			if(usleep(100000) == -1){
				std::cout << "usleep failed" << std::endl;
				return 1;
			}
		}
	
	} else if(strcmp(argv[1], "rate-limit") == 0) {
		if(argc != 3) {
			std::cout << "Format: bandwidth_monitor_cmd rate-limit [limit (in bytes per second)]" << std::endl;
			return 1;
		}
		
		Module module;
		
		ULong currentSeconds = 0;
		
		const ULong rateLimit = atoll(argv[2]);
		
		std::cout << "Running with rate limit of " << displayDataSize(rateLimit) << "/s..." << std::endl;
		
		while(true){
			Module module;

			const ULong seconds = module.getSeconds();
			
			if(seconds != currentSeconds){
				currentSeconds = seconds;
				
				module.setLimit(rateLimit);
			}
			
			if(usleep(100000) == -1){
				std::cout << "usleep failed" << std::endl;
				return 1;
			}
		}
		
	} else if(strcmp(argv[1], "get") == 0) {
		if(argc != 2) {
			std::cout << "Error: Too many parameters" << std::endl;
			return 1;
		}
		
		BandwidthFile bandwidthFile;
		
		Bandwidth bandwidth;
		
		if(!bandwidthFile.load(&bandwidth)){
			std::cout << "Failed to open bandwidth file" << std::endl;
			return 1;
		}
		
		const ULong totalUsage = bandwidth.down + bandwidth.up;
		
		printf("Total: %s, Down: %s, Up: %s\n", displayDataSize(totalUsage).c_str(), displayDataSize(bandwidth.down).c_str(), displayDataSize(bandwidth.up).c_str());
		
		Module module;
		
		std::cout << "Rate limit: " << displayDataSize(module.getLimit()) << "/s" << std::endl;
	} else if(strcmp(argv[1], "poll") == 0) {
		if(argc != 2) {
			std::cout << "Error: Too many parameters" << std::endl;
			return 1;
		}
		
		BandwidthFile bandwidthFile;
		
		Bandwidth bandwidth;
		
		while(true) {
			if(!bandwidthFile.load(&bandwidth)){
				std::cout << "Failed to open bandwidth file" << std::endl;
				return 1;
			}
			
			const ULong totalUsage = bandwidth.down + bandwidth.up;
			
			Module module;
			
			printf("     Module - Total: %s, Down: %s, Up: %s, Rate limit: %s/s        \r",
				displayDataSize(totalUsage).c_str(),
				displayDataSize(bandwidth.down).c_str(),
				displayDataSize(bandwidth.up).c_str(),
				displayDataSize(module.getLimit()).c_str());
			
			fflush(stdout);
			
			usleep(100000);
		}
	} else {
		std::cout << "Error: Unrecognised command '" << argv[1] << "'" << std::endl;
		return 1;
	}
	
	return 0;
}

