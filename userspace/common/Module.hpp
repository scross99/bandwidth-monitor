#ifndef MODULE_HPP
#define MODULE_HPP

#include <common/Bandwidth.hpp>
#include <common/ULong.hpp>

class Module{
	public:
		Bandwidth getBandwidth();
		
		ULong getSeconds();

		ULong getLimit();

		void setLimit(ULong limit);

};

#endif
