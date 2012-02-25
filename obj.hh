#ifndef OBJ_HH
#define OBJ_HH

class obj {
public:
	static constexpr volatile uint16_t *reg = (uint16_t *) 0x07000000;

	unsigned int index;
	uint16_t value0;
	uint16_t value1;
	uint16_t value2;

	obj(unsigned int index):
		index(index),
		value0(0),
		value1(0),
		value2(0)
	{
	}

	~obj()
	{
		reg[3 * index + 0] = value0;
		reg[3 * index + 1] = value1;
		reg[3 * index + 2] = value2;
	}

	obj &read()
	{
		value0 = reg[3 * index + 0];
		value1 = reg[3 * index + 1];
		value2 = reg[3 * index + 2];
		return *this;
	}

	obj &y(unsigned int y)
	{
		value0 |= (uint16_t) y;
		return *this;
	}

	obj &enable(bool x)
	{
		value0 |= (uint16_t) !x << 9;
		return *this;
	}

	obj &palette_16()
	{
		value0 |= (uint16_t) false << 13;
		return *this;
	}

	obj &palette_256()
	{
		value0 |= (uint16_t) true << 13;
		return *this;
	}

	obj &x(unsigned int x)
	{
		value1 |= (uint16_t) x;
		return *this;
	}

	obj &size(unsigned int x)
	{
		value1 |= (uint16_t) x << 14;
		return *this;
	}

	obj &tile(unsigned int x)
	{
		value2 |= (uint16_t) x;
		return *this;
	}
};

#endif
