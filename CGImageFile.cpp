#define _CRT_SECURE_NO_WARNINGS
#include "CGImageFile.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <string>

//---------------------------------------------------------------------------
// GENERIC INPUT/OUTPUT HELPERS
//---------------------------------------------------------------------------

typedef unsigned char cg_u8_t;
typedef unsigned short cg_u16_t;

static int write_u8(cg_u8_t value, FILE *f)
{
	if (fwrite(&value, sizeof(value),1,f) != 1)
		return -1;
	return 0;
}

static int write_u16le(cg_u16_t value, FILE *f)
{
	cg_u8_t buf[2];

	// little endian: least significant byte first
	buf[0]=(cg_u8_t) (value&0xff);
	buf[1]=(cg_u8_t) ((value>>8)&0xff);
  
	if (fwrite(buf, 2*sizeof(cg_u8_t),1,f) != 1)
		return -1;
	return 0;
}

static int read_u8(cg_u8_t *value, FILE *f)
{
	if (fread(value, sizeof(*value),1,f) != 1)
		return -1;
	return 0;
}

static int read_u16le(cg_u16_t *value, FILE *f)
{
	cg_u8_t buf[2];
  
	if (fread(buf, 2*sizeof(cg_u8_t),1,f) != 1) {
		*value=0;
		return -1;
	}

	// little endian: least significant byte first
	*value=((cg_u16_t)buf[1]) << 8 | (cg_u16_t)buf[0];
	return 0;
}

//---------------------------------------------------------------------------
// IMAGE CHANNEL CONVERSION
//
// Übung 11 - Aufgabe 1b  |  Funktion hinzugefügt
//---------------------------------------------------------------------------
static unsigned char * convert_to_ub4(unsigned char *data, int channels, int w, int h)
{
	unsigned char *newdata;
	const unsigned char *rptr;
	unsigned char *wptr;
	int x, y, c, ch;

	if (channels == 4)
		return data;

	if (!data)
		return NULL;

	if (channels > 4)
		ch = 4;
	else
		ch = channels;

	newdata = (unsigned char*)malloc(w*h * 4);
	if (!newdata) {
		fprintf(stderr, "convert_to_4ub: out of memory!\n");
		free(data);
		return NULL;
	}

	wptr = newdata;
	rptr = data;

	for (y = 0; y<h; y++) {
		for (x = 0; x<w; x++) {
			for (c = 0; c<ch; c++) {
				wptr[c] = rptr[c];
			}
			for (; c<4; c++) {
				if (c<3) {
					/* repeat red channel */
					wptr[c] = rptr[0];
				}
				else {
					/* set alpha to 255 */
					wptr[c] = 255;
				}
			}
			rptr += channels;
			wptr += 4;
		}
	}

	free(data);
	return newdata;
}

//-------------------------------------------------------------------------- -
// TGA
//
// Übung 11 - Aufgabe 1b  |  Funktionen hinzugefügt
//---------------------------------------------------------------------------

// TGA Fileheader

typedef struct
{
	cg_u8_t IDLength;
	cg_u8_t ColorMapType;
	cg_u8_t ImageType;
	cg_u16_t CMapStart;
	cg_u16_t CMapLength;
	cg_u8_t CMapDepth;
	cg_u16_t XOffset;
	cg_u16_t YOffset;
	cg_u16_t Width;
	cg_u16_t Height;
	cg_u8_t PixelDepth;
	cg_u8_t ImageDescriptor;
} tga_header_t;

typedef enum
{
	TGA_RGB,
	TGA_RGBA,
	TGA_BGR,
	TGA_BGRA,
	TGA_GRAY
} TGAFormat;

// ----------------------------- TGA writer ------------------------------ 

static void tga_write_pixel(FILE *f, const unsigned char *data, unsigned char bytes, int swap)
{
	if (swap) {
		unsigned char buf[4];
		if (bytes >= 3) {
			buf[0] = data[2];
			buf[1] = data[1];
			buf[2] = data[0];
			if (bytes == 4) {
				buf[3] = data[3];
				fwrite(buf, 4, 1, f);
			}
			else {
				fwrite(buf, 3, 1, f);
			}
		}
		else if (bytes == 1) {
			fwrite(data, 1, 1, f);
		}
	}
	else {
		fwrite(data, bytes, 1, f);
	}
}

static void tga_write_image_data(FILE *f, const unsigned char *data, unsigned char bytes, int swap, int w, int h)
{
	int x, y;

	assert(f);

	for (y = 0; y<h; y++) {
		for (x = 0; x<w; x++) {
			tga_write_pixel(f, data, bytes, swap);
			data += bytes;
		}
	}
}

enum tga_rle_encode_state
{
	TGA_RLE_EMPTY = 0,
	TGA_RLE_UNDECIDED,
	TGA_RLE_REPEAT,
	TGA_RLE_DIFFERENT,
	TGA_RLE_REPEAT_TO_DIFFERENT,
	TGA_RLE_DIFFERENT_TO_REPEAT
};

struct tga_rle_encode_s
{
	enum tga_rle_encode_state state;
	unsigned char pixel[4];
	unsigned char cnt;
	const unsigned char *ptr;
	const unsigned char *data;
	unsigned char bytes;
	int swap;
	FILE *f;
};

static void tga_rle_encode_init(struct tga_rle_encode_s *rle, const unsigned char *data, FILE *f, unsigned char bytes, int swap)
{
	rle->state = TGA_RLE_EMPTY;
	rle->cnt = 0;
	rle->ptr = data;
	rle->data = data;
	rle->f = f;
	rle->bytes = bytes;
	rle->swap = swap;
}

static void tga_rle_encode_flush(struct tga_rle_encode_s *rle)
{
	unsigned char i;
	switch (rle->state) {
	case TGA_RLE_EMPTY:
		return;
	case TGA_RLE_UNDECIDED:
		/* single pixel */
		write_u8(0, rle->f);
		tga_write_pixel(rle->f, rle->ptr, rle->bytes, rle->swap);
		rle->ptr += rle->bytes;
		rle->cnt = 0;
		rle->state = TGA_RLE_EMPTY;
		break;
	case TGA_RLE_DIFFERENT:
		/* sequence of different pixels */
		assert(rle->cnt > 0);
		write_u8(rle->cnt - 1, rle->f);
		for (i = 0; i<rle->cnt; i++) {
			tga_write_pixel(rle->f, rle->ptr, rle->bytes, rle->swap);
			rle->ptr += rle->bytes;
		}
		rle->cnt = 0;
		rle->state = TGA_RLE_EMPTY;
		break;
	case TGA_RLE_REPEAT:
		/* run sequence */
		assert(rle->cnt > 0);
		write_u8(0x80 | (rle->cnt - 1), rle->f);
		tga_write_pixel(rle->f, rle->ptr, rle->bytes, rle->swap);
		rle->ptr += ((size_t)rle->bytes) * ((size_t)rle->cnt);
		rle->cnt = 0;
		rle->state = TGA_RLE_EMPTY;
		break;
	case TGA_RLE_DIFFERENT_TO_REPEAT:
		/* sequence of different pixels */
		assert(rle->cnt > 2);
		write_u8(rle->cnt - 3, rle->f);
		for (i = 0; i<rle->cnt - 2; i++) {
			tga_write_pixel(rle->f, rle->ptr, rle->bytes, rle->swap);
			rle->ptr += rle->bytes;
		}
		rle->state = TGA_RLE_REPEAT;
		rle->cnt = 2;
		break;
	case TGA_RLE_REPEAT_TO_DIFFERENT:
		/* run sequence */
		assert(rle->cnt > 1);
		write_u8(0x80 | (rle->cnt - 2), rle->f);
		tga_write_pixel(rle->f, rle->ptr, rle->bytes, rle->swap);
		rle->ptr += ((size_t)rle->bytes) * ((size_t)rle->cnt - 1);
		rle->state = TGA_RLE_UNDECIDED;
		rle->cnt = 1;
		break;
	}
}

static void tga_rle_encode_pixel(struct tga_rle_encode_s *rle)
{
	int cmp;

	if (rle->state == TGA_RLE_EMPTY) {
		memcpy(rle->pixel, rle->data, rle->bytes);
		rle->cnt++;
		rle->data += rle->bytes;
		rle->state = TGA_RLE_UNDECIDED;
		return;
	}

	cmp = memcmp(rle->pixel, rle->data, rle->bytes);
	memcpy(rle->pixel, rle->data, rle->bytes);
	rle->data += rle->bytes;
	rle->cnt++;

	switch (rle->state) {
	case TGA_RLE_UNDECIDED:
		if (cmp) {
			rle->state = TGA_RLE_DIFFERENT;
		}
		else {
			rle->state = TGA_RLE_REPEAT;
		}
		break;
	case TGA_RLE_REPEAT:
		if (cmp) {
			/* end of run sequence */
			rle->state = TGA_RLE_REPEAT_TO_DIFFERENT;
			tga_rle_encode_flush(rle);
		}
		break;
	case TGA_RLE_DIFFERENT:
		if (!cmp) {
			/* run sequence detected */
			rle->state = TGA_RLE_DIFFERENT_TO_REPEAT;
			tga_rle_encode_flush(rle);
		}
		break;
	default:
		fprintf(stderr, "bug in TGA RLE encoder!\n");
	}

	if (rle->cnt == 128) {
		/* force end of sequence */
		tga_rle_encode_flush(rle);
	}
}

static void tga_write_image_data_rle(FILE *f, const unsigned char *data, unsigned char bytes, int swap, int w, int h)
{
	int x, y;
	struct tga_rle_encode_s rle;

	assert(f);

	for (y = 0; y<h; y++) {
		tga_rle_encode_init(&rle, data, f, bytes, swap);

		for (x = 0; x<w; x++) {
			tga_rle_encode_pixel(&rle);
		}
		tga_rle_encode_flush(&rle);
		data += (size_t)bytes * (size_t)w;
	}
}

static int
write_tga(FILE *f, TGAFormat format, int w, int h, const unsigned char *data, int rle)
{
	cg_u8_t depth;
	cg_u8_t image_desc = 0;
	int swap = 0;
	cg_u8_t img_type;

	switch (format) {
	case TGA_RGB:
		depth = 3;
		swap = 1;
		img_type = 2;
		break;
	case TGA_BGR:
		depth = 3;
		img_type = 2;
		break;
	case TGA_RGBA:
		depth = 4;
		img_type = 2;
		swap = 1;
		break;
	case TGA_BGRA:
		depth = 4;
		img_type = 2;
		break;
	case TGA_GRAY:
		depth = 1;
		img_type = 3;
		break;
	default:
		fprintf(stderr, "write_tga: unknown format %d, only RGB and RGBA and monochrome are supported\n", (int)format);
		return -4;
	}

	write_u8(0, f);
	write_u8(0, f);

	if (rle) {
		img_type |= 0x8;
	}

	write_u8(img_type, f); // type 
	write_u16le(0, f);
	write_u16le(0, f);
	write_u8(0, f);
	write_u16le(0, f);
	write_u16le(0, f);
	write_u16le((cg_u16_t)w, f);
	write_u16le((cg_u16_t)h, f);
	write_u8(depth * 8, f);
	if (depth > 3) {
		image_desc |= 0x8; /* alpha channel bit depth */
	}
	image_desc |= 0x20; /* we store image top-to-bottom */
	write_u8(image_desc, f);

	if (rle) {
		tga_write_image_data_rle(f, data, depth, swap, w, h);
	}
	else {
		if (swap) {
			tga_write_image_data(f, data, depth, 1, w, h);
		}
		else {
			/* fast path: raw dump */
			fwrite(data, w*h*depth, 1, f);
		}
	}

	if (ferror(f))
		return -1;

	return 0;
}

extern int
cgImageWriteTGA(const char *filename, int w, int h, const unsigned char *data, int rle)
{
	FILE *f;
	int res;

	f = fopen(filename, "w+b");
	if (!f) {
		fprintf(stderr, "cgImageWriteTGA: failed to open '%s' for writing\n", filename);
		return -1;
	}

	res = write_tga(f, TGA_RGBA, w, h, data, rle);
	fclose(f);
	return res;
}

// ----------------------------- TGA loader ------------------------------ 

static void tga_read_pixel(unsigned char *ptr, FILE *f, unsigned char bytes)
{
	char pixel[4];

	if (bytes >= 3) {
		fread(pixel, bytes, 1, f);
		ptr[0] = pixel[2];
		ptr[1] = pixel[1];
		ptr[2] = pixel[0];
		if (bytes == 4)
			ptr[3] = pixel[3];
	}
	else if (bytes == 1) {
		fread(ptr, 1, 1, f);
	}
}

static void tga_read_image_data(unsigned char *data, FILE *f, unsigned char bytes, unsigned short w, unsigned short h, int col_offset, int row_offset)
{
	unsigned short x, y;

	for (y = 0; y<h; y++) {
		for (x = 0; x<w; x++) {
			tga_read_pixel(data, f, bytes);
			data += col_offset;
		}
		data += row_offset;
	}
}

static int tga_read_image_data_rle(unsigned char *data, FILE *f, unsigned char bytes, unsigned short w, unsigned short h, int col_offset, int row_offset)
{
	unsigned short x, y;

	for (y = 0; y<h; y++) {
		cg_u8_t control;
		cg_u8_t i, cnt;

		x = 0;

		while (x < w) {
			if (read_u8(&control, f))
				return -1;
			cnt = (control & 0x7f) + 1;

			if (x + cnt > w) {
				fprintf(stderr, "crImageLoadTGA: RLE compression inconsistency!\n");
				cnt = (unsigned char)(w - x);
			}
			x += cnt;

			if (control & 0x80) {
				/* repeat follwoing pixel count times */
				unsigned char pixel[4];
				tga_read_pixel(pixel, f, bytes);
				for (i = 0; i<cnt; i++) {
					memcpy(data, pixel, bytes);
					data += col_offset;
				}
			}
			else {
				/* read cnt pixels */
				for (i = 0; i<cnt; i++) {
					tga_read_pixel(data, f, bytes);
					data += col_offset;
				}
			}
		}
		data += row_offset;
	}
	return 0;
}

static unsigned char * load_TGA(FILE *f, TGAFormat *format, int *w, int *h, unsigned char orientation)
{
	tga_header_t header;
	unsigned char bytes;
	unsigned char *data;
	unsigned char *ptr = NULL;
	int col_offset = 0;
	int row_offset = 0;
	int linelen;

	/* TGA-Header */
	read_u8(&header.IDLength, f);
	read_u8(&header.ColorMapType, f);
	read_u8(&header.ImageType, f);
	read_u16le(&header.CMapStart, f);
	read_u16le(&header.CMapLength, f);
	read_u8(&header.CMapDepth, f);
	read_u16le(&header.XOffset, f);
	read_u16le(&header.YOffset, f);
	read_u16le(&header.Width, f);
	read_u16le(&header.Height, f);
	read_u8(&header.PixelDepth, f);
	read_u8(&header.ImageDescriptor, f);

	if (header.IDLength) {
		if (fseek(f, (long)header.IDLength, SEEK_CUR)) {
			fprintf(stderr, "load_TGA: error skipping header\n");
			return NULL;
		}
	}

	if (header.ColorMapType) {
		long per_entry;

		/* SKIP color map / palette */
		per_entry = (long)header.CMapDepth / (long)8;
		if (header.CMapDepth % 8) {
			per_entry++;
		}
		if (per_entry && header.CMapLength) {
			if (fseek(f, (long)per_entry * (long)header.CMapLength, SEEK_CUR)) {
				fprintf(stderr, "load_TGA: error skipping palette\n");
				return NULL;
			}
		}
	}

	switch (header.ImageType & 0x7) {
	case 0:
		fprintf(stderr, "load_TGA: no image data\n");
		return NULL;
	case 1:
		fprintf(stderr, "load_TGA: indexed data not supported\n");
		return NULL;
	case 2:
	case 3:
		/* These formats are OK */
		break;
	default:
		fprintf(stderr, "load_TGA: image type unsupported, ignoring it\n");
		return NULL;
	}

	switch (header.PixelDepth) {
	case 24:
		*format = TGA_RGB;
		break;
	case 32:
		*format = TGA_RGBA;
		break;
	case 8:
		*format = TGA_GRAY;
		break;
	default:
		fprintf(stderr, "load_TGA: pixel depth %u unsupported, ignoring it\n", header.PixelDepth);
		return NULL;
	}

	*w = header.Width;
	*h = header.Height;

	bytes = header.PixelDepth / 8;
	linelen = (int)bytes * (int)header.Width;

	if (header.Width < 1 || header.Height < 1) {
		fprintf(stderr, "load_TGA: no image data\n");
		return NULL;
	}

	assert(bytes == 3 || bytes == 4 || bytes == 1);
	data = (unsigned char*)malloc(header.Width*header.Height*bytes);
	if (!data) {
		fprintf(stderr, "load_TGA: out of memory, ignoring it\n");
		return NULL;
	}

	/* image orientation */
	/* default orientation (0) is top-to-bottom, left-to-right,
	but user can reverse that using bit 0 (left/right) and
	bit 1 (top/bottom) of the orientation argument */
	switch (((header.ImageDescriptor >> 4) & 0x3) ^ orientation) {
	case 0:
		/* left to right, bottom to top */
		ptr = data + linelen * ((int)header.Height - 1);
		row_offset = -2 * linelen;
		col_offset = bytes;
		break;
	case 1:
		/* right to left, bottom to top */
		ptr = data + linelen * ((int)header.Height - 1) + (int)bytes*((int)header.Width - 1);
		col_offset = -bytes;
		row_offset = 0;
		break;
	case 2:
		/* left to right, top to bottom */
		ptr = data;
		col_offset = bytes;
		row_offset = 0;
		break;
	case 3:
		/* right to left, top to bottom  */
		ptr = data + (int)bytes*((int)header.Width - 1);
		col_offset = -bytes;
		row_offset = 2 * linelen;
		break;
	}

	/* read the pixels */
	if (header.ImageType & 0x8) {
		tga_read_image_data_rle(ptr, f, bytes, header.Width, header.Height, col_offset, row_offset);
	}
	else {
		tga_read_image_data(ptr, f, bytes, header.Width, header.Height, col_offset, row_offset);
	}

	if (ferror(f)) {
		fprintf(stderr, "load_TGA: read error, image data might be corrupted\n");
	}
	return data;
}

extern unsigned char *cgImageLoadTGA(const char *filename, int *w, int *h, unsigned char orientation)
{
	FILE *tga;
	unsigned char *data;
	TGAFormat format = TGA_RGBA;
	int channels;

	if (!(tga = fopen(filename, "rb"))) {
		fprintf(stderr, "cgImageLoadTGA: file '%s' can't be opened, ignoring it\n", filename);
		return NULL;
	}

	data = load_TGA(tga, &format, w, h, orientation);
	fclose(tga);

	switch (format) {
	case TGA_RGB:
	case TGA_BGR:
		channels = 3;
		break;
	case TGA_GRAY:
		channels = 1;
		break;
	default:
		channels = 4;
	}
	data = convert_to_ub4(data, channels, *w, *h);

	return data;
}

//---------------------------------------------------------------------------
// CG1
//
// Übung 11 - Aufgabe 1b  |  Funktionen hinzugefügt
//---------------------------------------------------------------------------

typedef enum
{
	CG1_IMAGE_FORMAT_UBYTE = 1
} cg1format_t;

typedef struct
{
	char hdr[4];
	cg_u16_t width;
	cg_u16_t height;
	cg_u8_t channels;
	cg_u8_t format;
} cg1hdr_t;

// ----------------------------- CG1 writer ------------------------------ 

static int
write_CG1(FILE *f, const unsigned char *data, int width, int height)
{
	static const char buf[4] = { 'C', 'G', '1', 'i' };
	cg1hdr_t hdr;
	const unsigned char *ptr;
	cg_u16_t x, y;
	cg_u8_t c;

	if (width > 0xffff || height > 0xffff) {
		fprintf(stderr, "save_CG1: image dimensions exceed file format limit\n");
		return -8;
	}

	hdr.width = (cg_u16_t)width;
	hdr.height = (cg_u16_t)height;
	hdr.channels = (cg_u8_t)4;
	hdr.format = CG1_IMAGE_FORMAT_UBYTE;

	fwrite(buf, 4, 1, f);
	write_u16le(hdr.width, f);
	write_u16le(hdr.height, f);
	write_u8(hdr.channels, f);
	write_u8(hdr.format, f);

	if (ferror(f)) {
		fprintf(stderr, "save_CG1: error writing file header!\n");
		return -9;
	}

	for (c = 0; c<hdr.channels; c++) {
		ptr = data + c;
		for (y = 0; y<hdr.height; y++) {
			for (x = 0; x<hdr.width; x++) {
				if (fwrite(ptr, 1, 1, f) != 1) {
					fprintf(stderr, "save_CG1: write error at channel %u, coord: %u,%u\n", c, x, y);
					return -10;
				}
				ptr += hdr.channels;
			}
		}
	}

	return 0;
}

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 2b  |  Funktion implementiert
// Hausaufgabe 1 - Aufgabe 2c  |  Bild in Ordner schreiben
// Übung 11      - Aufgabe 1b  |  Refaktorisierung
//---------------------------------------------------------------------------
extern int cgImageWriteCG1(const char *filename, const unsigned char *data, int width, int height)
{
	mkdir("Screenshots");
	
	int res;
	FILE *f;

	if (!(f = fopen(("Screenshots\\" + std::string(filename)).c_str(), "w+b"))) {
		fprintf(stderr, "cgImageWriteCG1: file '%s' can't be opened", ("Screenshots\\" + std::string(filename)).c_str());
		return -1;
	}

	res = write_CG1(f, data, width, height);
	fclose(f);

	return 0;
}

// ----------------------------- CG1 loader ------------------------------ 

static unsigned char *load_CG1(FILE *f, int *w, int *h, int *channels)
{
	unsigned char *data;
	cg1hdr_t hdr;
	unsigned char *ptr;
	cg_u16_t x, y;
	cg_u8_t c;

	*w = 0;
	*h = 0;
	*channels = 0;

	if (fread(hdr.hdr, 4, 1, f) != 1) {
		fprintf(stderr, "load_CG1: not a CG1 file!\n");
		return NULL;
	}

	if (hdr.hdr[0] != 'C' || hdr.hdr[1] != 'G' || hdr.hdr[2] != '1' || hdr.hdr[3] != 'i') {
		fprintf(stderr, "load_CG1: not a CG1 file!\n");
		return NULL;
	}

	read_u16le(&hdr.width, f);
	read_u16le(&hdr.height, f);
	read_u8(&hdr.channels, f);
	read_u8(&hdr.format, f);

	if (ferror(f)) {
		fprintf(stderr, "load_CG1: error reading CG1 header!\n");
		return NULL;
	}

	if (hdr.channels > 4) {
		fprintf(stderr, "load_CG1: invalid number of channels: %u, must be <= 4\n", hdr.channels);
	}

	switch (hdr.format) {
	case CG1_IMAGE_FORMAT_UBYTE:
		break;
	default:
		fprintf(stderr, "load_CG1: invalid format 0x%x, must be 0x1\n", hdr.format);
		return NULL;
	}

	//crDebug("load_CG1: %ux%u, %u channels, format: 0x%x",hdr.width, hdr.height,hdr.channels,hdr.format);
	if (hdr.width < 1 || hdr.height < 1) {
		fprintf(stderr, "load_CG1: empty image\n");
	}

	data = (unsigned char*)malloc((size_t)hdr.width * (size_t)hdr.height * hdr.channels);
	if (!data) {
		fprintf(stderr, "load_CG1: out of memory for %ux%u image", hdr.width, hdr.height);
		return NULL;
	}
	*w = (int)hdr.width;
	*h = (int)hdr.height;
	*channels = (int)hdr.channels;

	for (c = 0; c<hdr.channels; c++) {
		ptr = data + c;
		for (y = 0; y<hdr.height; y++) {
			for (x = 0; x<hdr.width; x++) {
				if (fread(ptr, 1, 1, f) != 1) {
					fprintf(stderr, "load_CG1: read error at channel %u, coord: %u,%u\n", c, x, y);
					return data;
				}
				ptr += hdr.channels;
			}
		}
	}

	return data;
}

extern unsigned char *cgImageLoadCG1(const char *filename, int *width, int *height)
{
	FILE *f;
	unsigned char *data;
	int channels;

	f = fopen(filename, "rb");
	if (!f) {
		fprintf(stderr, "cgImageLoadCG1: can't open '%s'\n", filename);
		return NULL;
	}
	data = load_CG1(f, width, height, &channels);
	fclose(f);

	data = convert_to_ub4(data, channels, *width, *height);
	return data;
}


//---------------------------------------------------------------------------
// IMAGE LOAD/WRITE API
//
// Übung 11 - Aufgabe 1b  |  Funktionen hinzugefügt
//---------------------------------------------------------------------------

extern int cgImageWrite(const char *filename, unsigned char *data, int width, int height)
{
	const char *ext;

	ext = strrchr(filename, '.');
	if (!ext) {
		fprintf(stderr, "cgImageWrite: can't determine type of '%s'\n", filename);
		return -1;
	}

	ext++;
	if (!strcmp(ext, "TGA") || !strcmp(ext, "tga")) {
		return cgImageWriteTGA(filename, width, height, data, 1);
	}
	else if (!strcmp(ext, "CG1") || !strcmp(ext, "cg1")) {
		return cgImageWriteCG1(filename, data, width, height);
	}

	fprintf(stderr, "cgImageWrite: unknown type '%s'\n", ext);
	return -2;
}

extern unsigned char *cgImageLoad(const char *filename, int *width, int *height)
{
	const char *ext;

	*width = 0;
	*height = 0;

	ext = strrchr(filename, '.');
	if (!ext) {
		fprintf(stderr, "cgImageLoad: can't determine type of '%s'\n", filename);
		return NULL;
	}

	ext++;
	if (!strcmp(ext, "TGA") || !strcmp(ext, "tga")) {
		return cgImageLoadTGA(filename, width, height, 0);
	}
	else if (!strcmp(ext, "CG1") || !strcmp(ext, "cg1")) {
		return cgImageLoadCG1(filename, width, height);
	}

	fprintf(stderr, "cgImageLoad: unknown type '%s'\n", ext);
	return NULL;
}

//---------------------------------------------------------------------------
// IMAGE CONVERTER
//
// Übung 11 - Aufgabe 1b  |  Funktion hinzugefügt
//---------------------------------------------------------------------------

extern int cgImageConvert(const char *filename_in, const char *filename_out)
{
	int w, h;
	unsigned char *data;
	int res = 0;

	data = cgImageLoad(filename_in, &w, &h);
	if (data) {
		res = cgImageWrite(filename_out, data, w, h);
		free(data);
	}
	else {
		res = -1;
	}

	return res;
}