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
void CGQuadric::createBox()
{
}
//---------------------------------------------------------------------------
void CGQuadric::createBox(int slicesX, int slicesY, int slicesZ)
{
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
void CGQuadric::createCylinder(int slices, int stacks)
{
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
//---------------------------------------------------------------------------
