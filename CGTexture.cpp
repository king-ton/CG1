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
// Übung 11 - Aufgabe 2c  |  Funktion implementiert für CG_NEAREST
//---------------------------------------------------------------------------
CGVec4 CGTexture2D::sample(const CGVec4 &texcoord) const
{
	CGVec4 sample;

	if (filterMode == CG_NEAREST) {
		int x = texcoord.elements[0] * width;
		int y = texcoord.elements[1] * height;
		sample = fetchTexel(x, y);
	}

	return sample;
}
