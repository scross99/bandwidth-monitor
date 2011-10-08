#include <fstream>

#include <common/Bandwidth.hpp>
#include <common/Module.hpp>
#include <common/ULong.hpp>

Bandwidth Module::getBandwidth(){
	std::ifstream streamUp("/sys/kernel/bandwidth/up");
	std::ifstream streamDown("/sys/kernel/bandwidth/down");
	ULong up = 0;
	streamUp >> up;

	ULong down = 0;
	streamDown >> down;

	return Bandwidth(down, up);
}

ULong Module::getSeconds(){
	std::ifstream stream("/sys/kernel/bandwidth/seconds");
	ULong seconds = 0;
	stream >> seconds;
	return seconds;
}

ULong Module::getLimit(){
	std::ifstream stream("/sys/kernel/bandwidth/limit");
	ULong limit = 0;
	stream >> limit;

	return limit;
}

void Module::setLimit(ULong limit){
	std::ofstream stream("/sys/kernel/bandwidth/limit");
	stream << limit;
}

