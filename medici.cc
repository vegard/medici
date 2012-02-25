#include <cstdint>

#include "backgrounds/title.cc"

extern "C"
int main(void)
{
	/* LCD off */
	*(volatile uint16_t *) 0x04000000 = (1 << 7);

	/* Set palette */
	for (unsigned int i = 0; i < title_palette_size; ++i)
		*((volatile uint16_t *) 0x05000000 + i) = title_palette[i];

	/* Copy background */
	for (unsigned int i = 0; i < 240 * 160 / 2; ++i)
		*((volatile uint16_t *) 0x06000000 + i) = ((uint16_t) title[2 * i + 1] << 8) + title[2 * i];

	/* Set BG mode */
	*(volatile uint16_t *) 0x04000000 = 4 | (1 << 6) | (1 << 10);

	while (true)
		;

	return 0;
}
