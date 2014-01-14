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
CGVec4 CGTexture2D::fetchTexel(int x, int y) const
{
	CGVec4 sample;
	// ...
	return sample;
}
//---------------------------------------------------------------------------
CGVec4 CGTexture2D::sample(const CGVec4 &texcoord) const
{
	CGVec4 sample;
	// ...
	return sample;
}
//---------------------------------------------------------------------------
