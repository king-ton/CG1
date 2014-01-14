#include "CGFrameBuffer.h"
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy

//---------------------------------------------------------------------------
// CGFRAMEBUFFER
//---------------------------------------------------------------------------
bool CGFrameBuffer::allocate(int width, int height)
{
	return (colorBuffer.reserve(width,height));
	// also reserve a depth buffer
	// ...
}
//---------------------------------------------------------------------------
int CGFrameBuffer::getWidth()
{
	return colorBuffer.getWidth();
}
//---------------------------------------------------------------------------
int CGFrameBuffer::getHeight()
{
	return colorBuffer.getHeight();
}
//---------------------------------------------------------------------------
// CGFRAMEBUFFER::CGBUFFER4UB
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer4UB::CGBuffer4UB()
{
	pBufferData = NULL;
}
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer4UB::~CGBuffer4UB()
{
	free(pBufferData);
}
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer4UB::reserve(int width, int height)
{
	this->width = width;
	this->height = height;

	// Uebung 01, Aufgabe 1a)
	// ...

	return true;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::get(int x, int y, float *rgba)
{
	// Uebung 01, Aufgabe 1c)
	// ...
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::set(int x, int y, const float *rgba)
{
	// Uebung 01, Aufgabe 1b)
	// ...
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::clear(const float *rgba)
{
	// Uebung 01, Aufgabe 1d)
	// ...
}
//---------------------------------------------------------------------------
unsigned char* CGFrameBuffer::CGBuffer4UB::getDataPointer()
{
	return pBufferData;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer4UB::getWidth()
{
	return width;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer4UB::getHeight()
{
	return height;
}
//---------------------------------------------------------------------------
// CGFRAMEBUFFER::CGBUFFER1F
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer1f::CGBuffer1f()
{
	pBufferData = NULL;
}
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer1f::~CGBuffer1f()
{
	free(pBufferData);
}
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer1f::reserve(int width, int height)
{
	// ...
	return true;
}
//---------------------------------------------------------------------------
float CGFrameBuffer::CGBuffer1f::get(int x, int y)
{
	// ...
	return 0.0f;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::set(int x, int y, const float z)
{
	// ...
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::clear()
{
	// ...
}
//---------------------------------------------------------------------------
float* CGFrameBuffer::CGBuffer1f::getDataPointer()
{
	// ...
	return NULL;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getWidth()
{
	// ...
	return 0;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getHeight()
{
	// ...
	return 0;
}
//---------------------------------------------------------------------------
