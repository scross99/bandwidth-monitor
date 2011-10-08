#ifndef BANDWIDTHFILE_HPP
#define BANDWIDTHFILE_HPP

#include <stdlib.h>

#include <cstdio>
#include <ctime>
#include <string>

#include <common/Bandwidth.hpp>
#include <common/Module.hpp>

// Loads from and saves to a file containing the bandwidth.
// This allows state to be saved between separate boots of
// the host machine.
class BandwidthFile{
	public:
		inline BandwidthFile(){
			char * homeDir = getenv("HOME");
			if(homeDir != 0){
				fileName_ = std::string(homeDir) + std::string("/.bandwidthMonitorData");
			}
		}
		
		inline bool load(Bandwidth * bandwidth){
			FILE * file = fopen(fileName_.c_str(), "rb");
			if(file != 0){
				size_t readSize = fread((void *) &(bandwidth->down), sizeof(ULong), 1, file);
				readSize += fread((void *) &(bandwidth->up), sizeof(ULong), 1, file);

				// Check this is the same day.
				time_t t;
				readSize += fread((void *) &t, sizeof(time_t), 1, file);
				tm * timeData = localtime(&t);
				int prevYearDay = timeData->tm_yday;
				int prevYear = timeData->tm_year;

				t = time(0);
				timeData = localtime(&t);
				
				if(feof(file) != 0 || ferror(file) != 0 || prevYearDay != timeData->tm_yday
						|| prevYear != timeData->tm_year){
					*bandwidth = Bandwidth(0ul, 0ul);
				}
				fclose(file);
				return true;
			}
			return false;
		}
		
		inline bool save(const Bandwidth& bandwidth){
			FILE * file = fopen(fileName_.c_str(), "wb");
			if(file != 0){
				size_t writeSize = fwrite((void *) &(bandwidth.down), sizeof(ULong), 1, file);
				writeSize += fwrite((void *) &(bandwidth.up), sizeof(ULong), 1, file);
				time_t t = time(0);
				writeSize += fwrite((void *) &t, sizeof(time_t), 1, file);
				fclose(file);
				return true;
			}
			return false;
		}
		
	private:
		std::string fileName_;

};

#endif
