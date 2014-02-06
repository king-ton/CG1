#include "CGTexture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
// CGTEXTURE2D
//---------------------------------------------------------------------------
CGTexture2D::CGTexture2D()
{
	width=height=0;
	filterMode=CG_NEAREST;
	wrapMode=CG_CLAMP;
	data=NULL;
}

//---------------------------------------------------------------------------
CGTexture2D::~CGTexture2D()
{
	free(data); data=NULL;
}

//---------------------------------------------------------------------------
void CGTexture2D::allocate(int width, int height, unsigned char* data4ub)
{
	this->width = width;
	this->height = height;
	data = (unsigned char*) realloc(data,width*height*4);
	memcpy(data,data4ub,width*height*4);
}

//---------------------------------------------------------------------------
void CGTexture2D::setFilterMode(int mode)
{
	filterMode = mode;
}

//---------------------------------------------------------------------------
void CGTexture2D::setWrapMode(int mode)
{
	wrapMode = mode;
}

//---------------------------------------------------------------------------
// Gibt Farbwert eines bestimmten Texels zurück
//
// Übung 11 - Aufgabe 2a  |  Funktion implementiert
//---------------------------------------------------------------------------
CGVec4 CGTexture2D::fetchTexel(int x, int y) const
{
	CGVec4 sample;
	
	if (wrapMode == CG_CLAMP) {
		if (x >= width)
			x = width - 1;
		if (y >= height)
			y = height - 1;
	}
	else if (wrapMode == CG_REPEAT) {
		if (x >= width)
			x = x % width;
		if (y >= height)
			y = y % height;
	}

	for (int i = 0; i < 4; i++)
		sample.elements[i] = data[y * width * 4 + x * 4 + i] / 255;

	return sample;
}

//---------------------------------------------------------------------------
// Gibt die Farbwerte an einer bestimmten Texel-Position zurück
//
// Übung 11 - Aufgabe 2c  |  Funktion implementiert für CG_NEAREST
// Übung 11 - Aufgabe 4a  |  Funktion implementiert für CG_LINEAR
//---------------------------------------------------------------------------
CGVec4 CGTexture2D::sample(const CGVec4 &texcoord) const
{
	CGVec4 sample;

	if (filterMode == CG_NEAREST) {
		int x = texcoord.elements[0] * width;
		int y = texcoord.elements[1] * height;
		sample = fetchTexel(x, y);
	}
	else if (filterMode == CG_LINEAR) {
		CGVec4 sample2[4];

		int x0 = texcoord.elements[0] * width - 0.5F;
		int y0 = texcoord.elements[1] * height - 0.5F;

		sample2[0] = fetchTexel(x0,		y0);
		sample2[1] = fetchTexel(x0 + 1,	y0);
		sample2[2] = fetchTexel(x0,		y0 + 1);
		sample2[3] = fetchTexel(x0 + 1,	y0 + 1);

		float a = texcoord.elements[0] * width - 0.5F - x0;
		float b = texcoord.elements[1] * height - 0.5F - y0;

		for (int i = 0; i < 4; i++)
			sample[i] = b * (a*sample2[3][i] + (1 - a) * sample2[2][i]) + (1 - b) * (a*sample2[1][i] + (1 - a) * sample2[0][i]);
	}

	return sample;
}
