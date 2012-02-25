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

	fprintf(out, "static uint8_t %s[] = {\n", argv[1]);

	png_bytep *row_pointers = png_get_rows(png, info);
	for (unsigned int y = 0; y < info->height; ++y) {
		png_bytep src_row = row_pointers[y];

		fprintf(out, "\t");
		for (unsigned int x = 0; x < info->width; ++x)
			fprintf(out, "0x%02x, ", src_row[x]);

		fprintf(out, "\n");

#if 0 /* Display in console */
		for (unsigned int x = 0; x < info->width; ++x)
			fprintf(stderr, "%c", isprint(32 + src_row[x]) ? 32 + src_row[x] : ' ');
		printf(stderr, "\n");
#endif
	}

	fprintf(out, "};\n");

	png_destroy_read_struct(&png, &info, NULL);
	fclose(fp);

	return 0;
}
