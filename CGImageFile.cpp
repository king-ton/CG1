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
// CG1
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 2b  |  Funktion implementiert
// Hausaufgebe 1 - Aufgabe 2c  |  Bild in Ordner schreiben
//---------------------------------------------------------------------------
extern int cgImageWriteCG1(const char *filename, const unsigned char *data, int width, int height)
{
	mkdir("Screenshots");
	
	FILE *file = fopen(("Screenshots\\" + std::string(filename)).c_str(), "wb");

	write_u8(0x43, file); write_u8(0x47, file); write_u8(0x31, file); write_u8(0x69, file);
	write_u16le(width, file); write_u16le(height, file);
	write_u8(4, file); write_u8(1, file);

	for (int i = 0; i < 4; ++i)
	for (int y = height - 1; y >= 0; --y)
	for (int x = 0; x < width; ++x)
		write_u8(data[4 * (x + y*width) + i], file);

	fclose(file);

	return 0;
}

