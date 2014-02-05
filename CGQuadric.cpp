#include <string.h> // for memcpy
#include <stdlib.h> // for malloc
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749
#endif

#include "CGQuadric.h"

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
CGQuadric::CGQuadric()
{
	m_positions = m_normals = m_colors = m_texcoords = NULL;
	m_vertexCount = 0;
	m_standardRGBA[0]=m_standardRGBA[1]=m_standardRGBA[2]=m_standardRGBA[3]=1.0f;
}

//---------------------------------------------------------------------------
CGQuadric::CGQuadric(const CGQuadric &q)
{
	m_vertexCount = q.m_vertexCount;

	m_positions = (float*) malloc(sizeof(float)*3*m_vertexCount);
	memcpy(m_positions,q.m_positions,sizeof(float)*3*m_vertexCount);
  
	m_normals = (float*) malloc(sizeof(float)*3*m_vertexCount);
	memcpy(m_normals,q.m_normals,sizeof(float)*3*m_vertexCount);
  
	m_colors = (float*) malloc(sizeof(float)*4*m_vertexCount);
	memcpy(m_colors,q.m_colors,sizeof(float)*4*m_vertexCount);

	m_texcoords = (float*) malloc(sizeof(float)*2*m_vertexCount);
	memcpy(m_texcoords,q.m_texcoords,sizeof(float)*2*m_vertexCount);
}

//---------------------------------------------------------------------------
CGQuadric::~CGQuadric()
{
	free(m_positions);
	free(m_normals);
	free(m_colors);
	free(m_texcoords);
}

//---------------------------------------------------------------------------
// GETTER
//---------------------------------------------------------------------------
int CGQuadric::getVertexCount()       { return m_vertexCount; }
float* CGQuadric::getPositionArray()  { return m_positions;   }
float* CGQuadric::getNormalArray()    { return m_normals;     }
float* CGQuadric::getColorArray()     { return m_colors;      }
float* CGQuadric::getTexCoordArray()  { return m_texcoords;   }

//---------------------------------------------------------------------------
void CGQuadric::setStandardColor(float r, float g, float b, float a)
{
	m_standardRGBA[0] = r;
	m_standardRGBA[1] = g;
	m_standardRGBA[2] = b;
	m_standardRGBA[3] = a;
}

//---------------------------------------------------------------------------
void CGQuadric::m_addTriangle(float x0, float y0, float z0, float nx0, float ny0, float nz0, 
                              float x1, float y1, float z1, float nx1, float ny1, float nz1, 
                              float x2, float y2, float z2, float nx2, float ny2, float nz2)
{
	// Increase array storage by 3 vertices.
	m_positions = (float*)realloc(m_positions,sizeof(float)*3*(m_vertexCount+3));
	m_normals   = (float*)realloc(m_normals,sizeof(float)*3*(m_vertexCount+3));
	m_colors    = (float*)realloc(m_colors,sizeof(float)*4*(m_vertexCount+3));
	m_texcoords = (float*)realloc(m_texcoords,sizeof(float)*2*(m_vertexCount+3));
  
	// Get the next free array position.
	float *p=m_positions+3*m_vertexCount,
		  *n=m_normals  +3*m_vertexCount,
		  *c=m_colors   +4*m_vertexCount,
		  *t=m_texcoords+2*m_vertexCount;

	// Set the respective values.
#define SET2(p,x,y)     { (p)[0]=x; (p)[1]=y;                     }
#define SET3(p,x,y,z)   { (p)[0]=x; (p)[1]=y; (p)[2]=z;           }
	SET3(p+0,x0,y0,z0) SET3(n+0,nx0,ny0,nz0) SET2(t+0,0,0) memcpy(c+0,m_standardRGBA,sizeof(float)*4);
	SET3(p+3,x1,y1,z1) SET3(n+3,nx1,ny1,nz1) SET2(t+2,0,0) memcpy(c+4,m_standardRGBA,sizeof(float)*4);
	SET3(p+6,x2,y2,z2) SET3(n+6,nx2,ny2,nz2) SET2(t+4,0,0) memcpy(c+8,m_standardRGBA,sizeof(float)*4);
#undef SET2
#undef SET3

	// Increase internal vertex count.
	m_vertexCount+=3;
}

//---------------------------------------------------------------------------
void CGQuadric::m_addQuad(float x0, float y0, float z0, float nx0, float ny0, float nz0, 
                          float x1, float y1, float z1, float nx1, float ny1, float nz1, 
                          float x2, float y2, float z2, float nx2, float ny2, float nz2, 
                          float x3, float y3, float z3, float nx3, float ny3, float nz3)
{
	m_addTriangle(x0,y0,z0,nx0,ny0,nz0,
				  x1,y1,z1,nx1,ny1,nz1,
				  x2,y2,z2,nx2,ny2,nz2);
	m_addTriangle(x0,y0,z0,nx0,ny0,nz0,
				  x2,y2,z2,nx2,ny2,nz2,
				  x3,y3,z3,nx3,ny3,nz3);
}

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 2.1  |  Funktion implementiert
//---------------------------------------------------------------------------
void CGQuadric::createBox()
{
	createBox(1, 1, 1);
}

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 2.1  |  Funktion implementiert
//---------------------------------------------------------------------------
void CGQuadric::createBox(int slicesX, int slicesY, int slicesZ)
{
	// FRONT + BACK
	for (int iX = 0; iX<slicesX; iX++) {
		for (int iY = 0; iY<slicesY; iY++) {
			float left = float(iX) / float(slicesX)*2.0f - 1.0f,
				right = float(iX + 1) / float(slicesX)*2.0f - 1.0f,
				bottom = float(iY) / float(slicesY)*2.0f - 1.0f,
				top = float(iY + 1) / float(slicesY)*2.0f - 1.0f,
				back = -1,
				front = 1;
			float u0 = float(iX) / float(slicesX),
				u1 = float(iX + 1) / float(slicesX),
				v0 = float(iY) / float(slicesY),
				v1 = float(iY + 1) / float(slicesY);
			m_addQuad(left, bottom, front, 0, 0, 1,
				right, bottom, front, 0, 0, 1,
				right, top, front, 0, 0, 1,
				left, top, front, 0, 0, 1);
			m_addQuad(left, bottom, back, 0, 0, -1,
				left, top, back, 0, 0, -1,
				right, top, back, 0, 0, -1,
				right, bottom, back, 0, 0, -1);
		}
	}

	// LEFT + RIGHT
	for (int iY = 0; iY<slicesY; iY++) {
		for (int iZ = 0; iZ<slicesZ; iZ++) {
			float left = -1,//float(iX)  /float(slicesX)*sizeX, 
				right = 1,//float(iX+1)/float(slicesX)*sizeX,
				bottom = float(iY) / float(slicesY)*2.0f - 1.0f,
				top = float(iY + 1) / float(slicesY)*2.0f - 1.0f,
				back = float(iZ) / float(slicesZ)*2.0f - 1.0f,
				front = float(iZ + 1) / float(slicesZ)*2.0f - 1.0f;
			float u0 = float(iY) / float(slicesY),
				u1 = float(iY + 1) / float(slicesY),
				v0 = float(iZ) / float(slicesZ),
				v1 = float(iZ + 1) / float(slicesZ);
			m_addQuad(left, bottom, back, -1, 0, 0,
				left, bottom, front, -1, 0, 0,
				left, top, front, -1, 0, 0,
				left, top, back, -1, 0, 0);
			m_addQuad(right, bottom, back, 1, 0, 0,
				right, top, back, 1, 0, 0,
				right, top, front, 1, 0, 0,
				right, bottom, front, 1, 0, 0);
		}
	}

	// TOP + BOTTOM
	for (int iZ = 0; iZ<slicesZ; iZ++) {
		for (int iX = 0; iX<slicesX; iX++) {
			float left = float(iX) / float(slicesX)*2.0f - 1.0f,
				right = float(iX + 1) / float(slicesX)*2.0f - 1.0f,
				bottom = -1,
				top = 1,
				back = float(iZ) / float(slicesZ)*2.0f - 1.0f,
				front = float(iZ + 1) / float(slicesZ)*2.0f - 1.0f;
			float u0 = float(iZ) / float(slicesZ),
				u1 = float(iZ + 1) / float(slicesZ),
				v0 = float(iX) / float(slicesX),
				v1 = float(iX + 1) / float(slicesX);
			m_addQuad(left, top, front, 0, 1, 0,
				right, top, front, 0, 1, 0,
				right, top, back, 0, 1, 0,
				left, top, back, 0, 1, 0);
			m_addQuad(left, bottom, front, 0, -1, 0,
				left, bottom, back, 0, -1, 0,
				right, bottom, back, 0, -1, 0,
				right, bottom, front, 0, -1, 0);
		}
	}
}

//---------------------------------------------------------------------------
void CGQuadric::createUVSphere(int slices, int stacks)
{
}

//---------------------------------------------------------------------------
void CGQuadric::createIcoSphere(int subdiv)
{
}
//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 2.2  |  Funktion implementiert
//---------------------------------------------------------------------------
void CGQuadric::createCylinder(int slices, int stacks)
{
	// create (stacks-1) rings
	for (int iSt = 0; iSt<stacks; iSt++) {
		float zb = float(iSt) / float(stacks);
		float zt = float(iSt + 1) / float(stacks);
		// with (slices-1) elements
		for (int iSl = 0; iSl<slices; iSl++) {
			float sliceRatioL = float(iSl) / float(slices);
			float sliceRatioR = float(iSl + 1) / float(slices);
			float cl = cosf(sliceRatioL*(float)M_PI*2.0f), sl = sinf(sliceRatioL*(float)M_PI*2.0f),
				cr = cosf(sliceRatioR*(float)M_PI*2.0f), sr = sinf(sliceRatioR*(float)M_PI*2.0f);
			m_addQuad(cl, sl, zb, cl, sl, 0,
				cr, sr, zb, cr, sr, 0,
				cr, sr, zt, cr, sr, 0,
				cl, sl, zt, cl, sl, 0);
		}
	}
}

//---------------------------------------------------------------------------
void CGQuadric::createDisk(int slices, int loops)
{
}

//---------------------------------------------------------------------------
void CGQuadric::createCone(int slices, int stacks)
{
}

//---------------------------------------------------------------------------
void CGQuadric::createCone(float maxHeight, int slices, int stacks)
{
}
