#ifndef BANDWIDTH_HPP
#define BANDWIDTH_HPP

#include <common/ULong.hpp>

struct Bandwidth{
	ULong down;
	ULong up;

	inline Bandwidth() : down(0ul), up(0ul){ }

	inline Bandwidth(ULong d, ULong u) : down(d), up(u){ }

	inline bool operator==(const Bandwidth& bandwidth) const{
		return down == bandwidth.down && up == bandwidth.up;
	}

	inline bool operator!=(const Bandwidth& bandwidth) const{
		return !((*this) == bandwidth);
	}

	inline void operator+=(const Bandwidth& bandwidth){
		down += bandwidth.down;
		up += bandwidth.up;
	}

	inline Bandwidth operator+(const Bandwidth& bandwidth) const{
		return Bandwidth(down + bandwidth.down, up + bandwidth.up);
	}

	inline Bandwidth operator-(const Bandwidth& bandwidth) const{
		return Bandwidth(down - bandwidth.down, up - bandwidth.up);
	}
};

#endif
