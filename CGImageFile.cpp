#define _CRT_SECURE_NO_WARNINGS
#include "CGImageFile.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

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

extern int cgImageWriteCG1(const char *filename, const unsigned char *data, int width, int height)
{
	// ...

	return -1;
}

