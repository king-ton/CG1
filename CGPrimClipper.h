#ifndef CG_PRIM_CLIPPER_H
#define CG_PRIM_CLIPPER_H

#include "CG.h"

//----------------------------------------------------------------------------
// Primitive clipper works in homogeneous clip space.
//----------------------------------------------------------------------------
class CGPrimClipper
{
public:
	// Bit mask, each bit for one clip boundary
	static unsigned int ClipBits;

	// Pointer to output vertices.
	static const CGVertexVaryings* outBuf() {return &sm_voutBuf[0];}

	// A point clipper, it does actually culling
	static int clipPoint(CGVertexVaryings *vinBuf, int vinCnt);

	// A line clipper
	// Return the vertex count after clipping.
	static int clipLine(CGVertexVaryings *vinBuf, int vinCnt);

	// A polygon clipper
	// Return the vertex count after clipping.
	static int clipPoly(CGVertexVaryings *vinBuf, int vinCnt);

private:
	static CGVertexVaryings sm_voutBuf[CG_MAX_VERTICES_IN_PIPELINE];  //output buffer, it is also the internal working buffer

	// Liang-Barsky line-clipping
	static int clipLine_LB(CGVertexVaryings *vinBuf, int vinCnt);

	// Sutherland-Hodgman polygon-clipping
	static int clipPoly_SH(CGVertexVaryings *vinBuf, int vinCnt);

	// Sutherland-Hodgman polygon-clipping against single clipping plane
	// voutBufSize is the maximal size of the voutBuf.
	// Return the number of output vertices.
	static int clipPoly_SH_single(unsigned int clipBound, const CGVertexVaryings *vinBuf, int nvin, CGVertexVaryings *voutBuf);
};

#endif /* !CG_PRIM_CLIPPER_H */
