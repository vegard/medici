#include <cstdint>

#include "dispcnt.hh"

#include "backgrounds/title.cc"

extern "C"
int main(void)
{
	/* LCD off */
	dispcnt()
		.blank(true);

	/* Set palette */
	for (unsigned int i = 0; i < title_palette_size; ++i)
		*((volatile uint16_t *) 0x05000000 + i) = title_palette[i];

	/* Copy background */
	for (unsigned int i = 0; i < 240 * 160 / 2; ++i)
		*((volatile uint16_t *) 0x06000000 + i) = ((uint16_t) title[2 * i + 1] << 8) + title[2 * i];

	/* Set BG mode */
	dispcnt()
		.mode(4)
		.bg2(true);

	while (true)
		;

	return 0;
}
