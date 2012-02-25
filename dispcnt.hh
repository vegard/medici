#ifndef DISPCNT_HH
#define DISPCNT_HH

#include <cstdint>

class dispcnt {
public:
	static constexpr volatile uint16_t *reg = (uint16_t *) 0x04000000;

	uint16_t value;

	dispcnt():
		value(0)
	{
	}

	~dispcnt()
	{
		*reg = value;
	}

	dispcnt &read()
	{
		value = *reg;
		return *this;
	}

	dispcnt &mode(unsigned int i)
	{
		value |= i;
		return *this;
	}

	dispcnt &obj_mapping_1d(bool x)
	{
		value |= (uint16_t) x << 6;
		return *this;
	}

	dispcnt &blank(bool x)
	{
		value |= (uint16_t) x << 7;
		return *this;
	}

	dispcnt &bg2(bool x)
	{
		value |= (uint16_t) x << 10;
		return *this;
	}

	dispcnt &obj(bool x)
	{
		value |= (uint16_t) x << 12;
		return *this;
	}
};

#endif
