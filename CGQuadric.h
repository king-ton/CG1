#ifndef CG_QUADRIC_H
#define CG_QUADRIC_H

#include "CG.h"
//---------------------------------------------------------------------------
// CGQuadric-Class
// creates and holds polygonal (triangulated) discretizations
// of objects which can be described through quadratic equations.
//
// Hausaufgabe 3 - Aufgabe 2.5.1  |  Methode subdivideTriangle
//---------------------------------------------------------------------------
class CGQuadric
{
public:
	/// Empty constructor.
	CGQuadric();
	/// Copy constructor.
	CGQuadric(const CGQuadric &q);
	/// Destructor.
	~CGQuadric();
	/// Vertex count getter:
	int getVertexCount();
	/// Vertex positions getter:
	float* getPositionArray();
	/// Vertex normals getter:
	float* getNormalArray();
	/// Vertex colors getter:
	float* getColorArray();
	/// Vertex texture coordinates getter:
	float* getTexCoordArray();
	/// Set the color (before!) creating the object.
	void setStandardColor(float r, float g, float b, float a=1);

	/// Create a cubical object.
	void createBox();
	void createBox(int slicesX, int slicesY, int slicesZ);
	/// Create a sphere object.
	void createUVSphere(int slices, int stacks);
	void createIcoSphere(int subdiv);
	/// Create a cylinder object.
	void createCylinder(int slices, int stacks);
	/// Create a disk object.
	void createDisk(int slices, int loops);
	/// Create a cone object.
	void createCone(int slices, int stacks);
	void createCone(float maxHeight, int slices, int stacks);

private:
	/// Internal method to add single triangle.
	void m_addTriangle(float x0, float y0, float z0, float nx0, float ny0, float nz0, 
					   float x1, float y1, float z1, float nx1, float ny1, float nz1, 
					   float x2, float y2, float z2, float nx2, float ny2, float nz2);

	/// Internal method to add single quad.
	void m_addQuad(float x0, float y0, float z0, float nx0, float ny0, float nz0, 
				   float x1, float y1, float z1, float nx1, float ny1, float nz1, 
				   float x2, float y2, float z2, float nx2, float ny2, float nz2, 
				   float x3, float y3, float z3, float nx3, float ny3, float nz3);

	/// Subdive triangle into 4 smaller ones.
	void m_subdivideTriangle(int subdiv,	float r,	float g,	float b,
											float x0,	float y0,	float z0,
											float x1,	float y1,	float z1,
											float x2,	float y2,	float z2);

	/// Internal pointers to data.
	float *m_positions, *m_normals, *m_colors, *m_texcoords;

	/// Internal vertex count.
	int m_vertexCount;
	float m_standardRGBA[4];
};
//---------------------------------------------------------------------------

#endif
