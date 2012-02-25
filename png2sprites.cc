#include <cstdint>
#include <stdexcept>

extern "C" {
#include <png.h>
}

int main(int argc, char *argv[])
{
	if (argc != 4)
		throw std::runtime_error("Wrong number of arguments");

	fprintf(stderr, "Converting %s\n", argv[2]);

	FILE *fp = fopen(argv[2], "rb");
	if (!fp)
		throw std::runtime_error("fopen");

	FILE *out = fopen(argv[3], "w+");
	if (!out)
		throw std::runtime_error("fopen");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if (!png)
		throw std::runtime_error("png_create_read_struct");

	png_infop info = png_create_info_struct(png);
	if (!info)
		throw std::runtime_error("png_create_info_struct");

	png_init_io(png, fp);
	png_read_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

	fprintf(stderr, "Bit depth: %u\n", info->bit_depth);
	fprintf(stderr, "Size: %lux%lu\n", info->width, info->height);
	fprintf(stderr, "Channels: %u\n", info->channels);
	fprintf(stderr, "Palette size: %u\n", info->num_palette);

	if (info->bit_depth != 8)
		throw std::runtime_error("Unsupported bit depth");
	if (info->channels != 1)
		throw std::runtime_error("Unsupported number of channels");

	fprintf(out, "static const unsigned int %s_palette_size = %uU;\n",
		argv[1], info->num_palette);

	fprintf(out, "static uint16_t %s_palette[] = {\n", argv[1]);
	for (unsigned int i = 0; i < info->num_palette; ++i) {
		fprintf(out, "\t0x%04x,\n",
			(uint16_t) ((info->palette[i].red >> 3) << 0)
			+ (uint16_t) ((info->palette[i].green >> 3) << 5)
			+ (uint16_t) ((info->palette[i].blue >> 3) << 10));
	}
	fprintf(out, "};\n");

	if (info->width % 32 != 0)
		throw std::runtime_error("Width must be a multiple of 32");
	if (info->height % 32 != 0)
		throw std::runtime_error("Height must be a multiple of 32");

	fprintf(out, "static uint32_t %s[] = {\n", argv[1]);

	png_bytep *row_pointers = png_get_rows(png, info);

	for (unsigned int y = 0; y < info->height; y += 32) {
		for (unsigned int x = 0; x < info->width; x += 32) {
			for (unsigned int y2 = 0; y2 < 32; y2 += 8) {
				for (unsigned int x2 = 0; x2 < 32; x2 += 8) {
					fprintf(out, "\t");

					for (unsigned int y3 = 0; y3 < 8; ++y3) {
						for (unsigned int x3 = 0; x3 < 8; x3 += 4) {
							uint8_t *p = &row_pointers[y + y2 + y3][x + x2 + x3];
							fprintf(out, "0x%08x, ",
								(uint32_t) p[0]
								| (uint32_t) p[1] << 8
								| (uint32_t) p[2] << 16
								| (uint32_t) p[3] << 24);
						}
					}

					fprintf(out, "\n");
				}
			}

#if 0 /* Display in console */
			fprintf(stderr, "sprite at %u, %u:\n", x, y);
			for (unsigned int y2 = 0; y2 < 32; ++y2) {
				for (unsigned int x2 = 0; x2 < 32; ++x2) {
					uint8_t *p = &row_pointers[y + y2][x + x2];
					fprintf(stderr, "%c", isprint(32 + *p) ? 32 + *p : ' ');
				}

				fprintf(stderr, "\n");
			}
#endif
		}
	}

	fprintf(out, "};\n");

	png_destroy_read_struct(&png, &info, NULL);
	fclose(fp);

	return 0;
}
