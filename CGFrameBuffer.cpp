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
// �bung 01 - Aufgabe 1a  |  Speicherbereich reservieren f�r Color-Buffer
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer4UB::reserve(int width, int height)
{
	this->width = width;
	this->height = height;

	// Speicherbereich f�r Color-Buffer erzeugen
	pBufferData = (unsigned char*)malloc(4 * width * height);

	return true;
}

//---------------------------------------------------------------------------
// �bung 01 - Aufgabe 1c  |  Gibt einen Wert des Color-Buffers zur�ck
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::get(int x, int y, float *rgba)
{
	// Gibt den Farbwert an der Stelle (x, y) zur�ck
	for (int i = 0; i < 4; i++)
		rgba[i] = pBufferData[4 * (y * width + x) + i] / 255.0F;
}

//---------------------------------------------------------------------------
// �bung 01 - Aufgabe 1b  |  Schreiben eines Farbwertes in den Color-Buffer
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::set(int x, int y, const float *rgba)
{
	// Schreibt einen Farbwert an die Stelle (x, y)
	for (int i = 0; i < 4; i++)
		pBufferData[4 * (y * width + x) + i] = rgba[i] * 255.0F;
}

//---------------------------------------------------------------------------
// �bung 01 - Aufgabe 1d  |  Setzt den kompletten Color-Buffer auf gleichen 
//						  |  Wert 
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::clear(const float *rgba)
{
	for (int iY = 0; iY < height; iY++)
	for (int iX = 0; iX < width; iX++)
		set(iX, iY, rgba);
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
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer1f::reserve(int width, int height)
{
	this->width = width;
	this->height = height;

	pBufferData = (float*)malloc(width*height*sizeof(float));
	return true;
}
//---------------------------------------------------------------------------
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
float CGFrameBuffer::CGBuffer1f::get(int x, int y)
{
	return pBufferData[(y*width + x)];
}
//---------------------------------------------------------------------------
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::set(int x, int y, const float z)
{
	pBufferData[(y*width + x)] = z;
}
//---------------------------------------------------------------------------
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::clear()
{
	for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++)
		set(j, i, 2);
}
//---------------------------------------------------------------------------
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
float* CGFrameBuffer::CGBuffer1f::getDataPointer()
{
	return pBufferData;
}
//---------------------------------------------------------------------------
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getWidth()
{
	return width;
}
//---------------------------------------------------------------------------
// �bung 05 - Aufgabe 3b  |  Methode implementiert
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getHeight()
{
	return height;
}
