#include <cstdint>

#include "dispcnt.hh"
#include "obj.hh"

#include "backgrounds/title.cc"
#include "sprites/leonardo.cc"

static inline void vblank_wait()
{
#ifndef __thumb__
	asm volatile ("swi #0x50000"
		:
		:
		: "memory", "r0", "r1");
#else
	asm volatile ("swi #0x5"
		:
		:
		: "memory", "r0", "r1");
#endif
}

static enum {
	LEFT,
	RIGHT,
	UP,
	DOWN,
} direction = RIGHT;

static bool moving = false;

static unsigned int x = 0;
static unsigned int y = 0;

static void vblank_irq()
{
	static unsigned int update = 0;
	static unsigned int frame = 0;

	static const unsigned int indexes[] = {0, 1, 0, 2};

	obj(0)
		.read()
		.tile(512 + 32 * ((3 * direction) + indexes[frame]))
		.x(x)
		.y(y);

	if (update % 2 == 0) {
		if (moving) {
			switch (direction) {
			case LEFT:
				if (--x == -32)
					x = 239;
				break;
			case RIGHT:
				if (++x == 240)
					x = -31;
				break;
			case UP:
				if (--y == -32)
					y = 159;
				break;
			case DOWN:
				if (++y == 160)
					y = -31;
				break;
			}
		}
	}

	if (++update == 8) {
		update = 0;

		if (moving) {
			if (++frame == 4)
				frame = 0;
		}
	}

	/* Deal with keypad changes */
	static uint16_t keypad_prev = 0;
	uint16_t keypad = ~*(volatile uint16_t *) 0x04000130;
	uint16_t keypad_pressed = ~keypad_prev & keypad;
	uint16_t keypad_released = keypad_prev & ~keypad;

	if (keypad & (1 << 4)) {
		/* Right */
		direction = RIGHT;
		moving = true;
	} else if (keypad & (1 << 5)) {
		/* Left */
		direction = LEFT;
		moving = true;
	} else if (keypad & (1 << 6)) {
		/* Up */
		direction = UP;
		moving = true;
	} else if (keypad & (1 << 7)) {
		/* Down */
		direction = DOWN;
		moving = true;
	} else if (keypad_released & 0xf0) {
		moving = false;
		frame = 0;
	}

	if (keypad_pressed & (1 << 8)) {
		/* R */
	}

	if (keypad_pressed & (1 << 9)) {
		/* L */
	}

	keypad_prev = keypad;
}

extern void irq()
{
	uint16_t flags = *(volatile uint16_t *) 0x04000202;

	if (flags & (1 << 0))
		vblank_irq();

	/* Acknowledge IRQ */
	*(volatile uint16_t *) 0x03007ff8 |= flags;
	*(volatile uint16_t *) 0x04000202 = flags;
}

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

	/* Set up interrupt handler */
	*(volatile void **) 0x03007ffc = (void *) &irq;

	/* Request V-blank interrupt */
	*(volatile uint16_t *) 0x04000004 = (1 << 3);
	*(volatile uint16_t *) 0x04000200 |= (1 << 0);

	/* Request keypad interrupt */
	*(volatile uint16_t *) 0x04000132 = /* Start: */ (1 << 3) | (1 << 14);
	*(volatile uint16_t *) 0x04000200 |= (1 << 12);

	/* Master interrupt enable */
	*(volatile uint16_t *) 0x04000208 = 1;

	/* Display leonardo sprite */
	obj(0)
		.tile(512)
		.palette_256()
		.size(2)
		.x(0)
		.y(80)
		.enable(true);

	/* Set BG mode */
	dispcnt()
		.mode(4)
		.obj_mapping_1d(true)
		.bg2(true)
		.obj(true);

	while (true)
		vblank_wait();

	return 0;
}
