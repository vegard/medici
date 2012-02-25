#include <cstdint>

#include "dispcnt.hh"
#include "obj.hh"

#include "backgrounds/title.cc"
#include "sprites/leonardo.cc"

extern "C"
int main(void)
{
	/* LCD off */
	dispcnt()
		.blank(true);

	/* Set BG palette */
	for (unsigned int i = 0; i < title_palette_size; ++i)
		*((volatile uint16_t *) 0x05000000 + i) = title_palette[i];

	/* Copy BG data */
	for (unsigned int i = 0; i < 240 * 160 / 4; ++i)
		*((volatile uint32_t *) 0x06000000 + i) = title[i];

	/* Set OBJ palette */
	for (unsigned int i = 0; i < leonardo_palette_size; ++i)
		*((volatile uint16_t *) 0x05000200 + i) = leonardo_palette[i];

	/* Copy OBJ tiles */
	for (unsigned int i = 0; i < 32 * 32 * 12 / 4; ++i)
		*((volatile uint32_t *) 0x06014000 + i) = leonardo[i];

	/* Clear OBJ attributes */
	for (unsigned int i = 0; i < 128; ++i)
		obj(i).enable(false);

	/* Display leonardo sprite */
	obj(0)
		.tile(512)
		.palette_256()
		.size(2)
		.x(200)
		.y(80)
		.enable(true);

	/* Set BG mode */
	dispcnt()
		.mode(4)
		.obj_mapping_1d(true)
		.bg2(true)
		.obj(true);

	while (true)
		;

	return 0;
}
