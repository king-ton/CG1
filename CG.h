#ifndef CG_H
#define CG_H

//---------------------------------------------------------------------------

enum CGSpaceDimensions {X=0, Y, Z, W};
enum CGColorDimensions {R=0, G, B, A};

/// Simple vector container, four elements.
struct CGVec4
{
	float elements[4];
	const float & operator[] (int i) const { return elements[i]; }
	float & operator[] (int i) { return elements[i]; }
	void set(float a, float b, float c, float d)
	{
		elements[0]=a;
		elements[1]=b;
		elements[2]=c;
		elements[3]=d;
	}
	void set(const CGVec4& vec)
	{
		elements[0]=vec.elements[0];
		elements[1]=vec.elements[1];
		elements[2]=vec.elements[2];
		elements[3]=vec.elements[3];
	}
};

/// Simple (integer) vector container, two elements.
struct CGVec2i
{
	int elements[2];
	int & operator[] (int i) { return elements[i]; }
	const int & operator[] (int i) const { return elements[i]; }
	void set(int a, int b)
	{
		elements[0]=a;
		elements[1]=b;
	}
	void set(const CGVec2i& vec)
	{
		elements[0]=vec.elements[0];
		elements[1]=vec.elements[1];
	}
};

//---------------------------------------------------------------------------
// DEFINES
//---------------------------------------------------------------------------

typedef unsigned int CGenum;

/// Primitive types.
#define CG_POINTS         0x0000
#define CG_LINES          0x0001
#define CG_LINE_LOOP      0x0002
#define CG_LINE_STRIP     0x0003
#define CG_TRIANGLES      0x0004
#define CG_TRIANGLE_STRIP 0x0005
#define CG_TRIANGLE_FAN   0x0006
#define CG_QUADS          0x0007
#define CG_QUAD_STRIP     0x0008
#define CG_POLYGON        0x0009

/// Polygon modes
#define CG_LINE           0x1B01
#define CG_FILL           0x1B02

/// Capabilities.
#define CG_CULL_FACE      0x0B44
#define CG_DEPTH_TEST     0x0B71
#define CG_BLEND          0x0BE2
#define CG_USE_BRESENHAM  0x0B60 // ! GL_FOG !

/// Bit values for cgClear
#define CG_COLOR_BUFFER_BIT 0x00004000
#define CG_DEPTH_BUFFER_BIT 0x00000100

/// Key mappings
#define CG_KEY_LEFT       0x0010
#define CG_KEY_UP         0x0011
#define CG_KEY_RIGHT      0x0012
#define CG_KEY_DOWN       0x0013
#define CG_KEY_PAGE_UP    0x0014
#define CG_KEY_PAGE_DOWN  0x0015
#define CG_KEY_HOME       0x0016
#define CG_KEY_END        0x0017
#define CG_KEY_INSERT     0x0018

/// Limits
#define CG_MAX_VERTICES_IN_PIPELINE    32
#define CG_MAX_FRAGMENTS_IN_PIPELINE 4096
#define CG_MAX_TEXTURE_UNITS 4

//---------------------------------------------------------------------------

#endif
