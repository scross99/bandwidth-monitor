#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <sstream>
#include <string>

// Changes raw bytes value into a clearer visual format.
inline std::string displayDataSize(unsigned long bytes){
	const double B = 1.0;
	const double KB = B * 1024.0;
	const double MB = KB * 1024.0;
	const double GB = MB * 1024.0;
	const double TB = GB * 1024.0;

	std::ostringstream stream;
	if(bytes >= TB){
		stream << (double(bytes) / TB) << "TB";
	}else if(bytes >= GB){
		stream << (double(bytes) / GB) << "GB";
	}else if(bytes >= MB){
		stream << (double(bytes) / MB) << "MB";
	}else if(bytes >= KB){
		stream << (double(bytes) / KB) << "KB";
	}else{
		stream << (double(bytes) / B) << "B";
	}
	
	return stream.str();
}

#endif
