#include "CGContext.h"
#include "CGPrimClipper.h"
#include <iostream>
#include <cassert>
#include <limits>

//---------------------------------------------------------------------------
// CGCONTEXT (PRIMITIVE OPERATIONS)
//---------------------------------------------------------------------------
int CGContext::m_cgClipPrimitive()
{
	// take all vertices from m_pipelineVertexVaryings and clip (in clip space)
	// overwrite with new vertices. Could also handle QUADS and POLYGONS here!
	int newPrimCount = 1;
	switch(m_primitiveMode) {
		case CG_POINTS:
			m_pipelineVerticesCount =
				CGPrimClipper::clipPoint(m_pipelineVertexVaryings, m_pipelineVerticesCount);
			newPrimCount = m_pipelineVerticesCount;
			break;
		case CG_LINES:
			m_pipelineVerticesCount =
				CGPrimClipper::clipLine(m_pipelineVertexVaryings, m_pipelineVerticesCount);
			newPrimCount = m_pipelineVerticesCount/2;
			break;
		case CG_TRIANGLES:
			// Clipped vertices need to be re-assembled into triangles
			newPrimCount = CGPrimClipper::clipPoly(m_pipelineVertexVaryings, m_pipelineVerticesCount)-2;
			m_pipelineVerticesCount = 0; //reset the pipeline
			for (int i=0; i<newPrimCount; ++i) { //as triangle fan
				m_pipelineVertexVaryings[m_pipelineVerticesCount++]=CGPrimClipper::outBuf()[0];
				m_pipelineVertexVaryings[m_pipelineVerticesCount++]=CGPrimClipper::outBuf()[i+1];
				m_pipelineVertexVaryings[m_pipelineVerticesCount++]=CGPrimClipper::outBuf()[i+2];
			}
			break;
		// Insert other primitive types here.
		}

	// Returns the new primitive count
	return newPrimCount;
}
//---------------------------------------------------------------------------
bool CGContext::m_cgBFCullTriangle(CGVec4 &vertex0Position, 
                                   CGVec4 &vertex1Position, 
                                   CGVec4 &vertex2Position)
{
	// ...
	return false; // Not culled.
}
//---------------------------------------------------------------------------
// CGCONTEXT RASTERIZERS
//---------------------------------------------------------------------------
void CGContext::m_cgRasterize(int primitiveNumber)
{
	// Rasterize one primitive with the correct rasterizer.
	// Note: without clipping, this is always primitiveNumber=0, using
	// vertices (0) for points, (0,1) for lines and (0,1,2) for triangles.
	// BUT: with clipping, more primitives might be created and vertices
	// (N*i+0, N*i+1, N*i+2) must be used (where N=3 for triangles).
  
	switch(m_primitiveMode) {
		case CG_POINTS:
			m_cgRasterizePoint(primitiveNumber*1);
			break;
		case CG_LINES:
			if (m_capabilities.useBresenham) 
				m_cgRasterizeLineBresenham(primitiveNumber*2);
			else 
				m_cgRasterizeStupidLine(primitiveNumber*2);
			break;
		case CG_TRIANGLES:
			if (m_polygonMode == CG_LINE)
				m_cgRasterizeWireTriangle(primitiveNumber*3);
			else
				m_cgRasterizeTriangle(primitiveNumber*3);
			break;
		// Insert other rasterizers here.
	}

	// Process any remaining fragments
	m_cgFlushFragments();
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizePoint(int pipelineVertexOffset)
{
	// This primitive produces only one fragment.

	// We are interested only in one vertex data:
	CGVertexVaryings &vertex0 = m_pipelineVertexVaryings[pipelineVertexOffset+0];

	// Initialize the fragment.
	CGFragmentData fragment;
	fragment.set(vertex0);

	// And set coordinates. (SetFragment initializes just the attributes + varyings).
	fragment.coordinates.set((int) vertex0.varyings[CG_POSITION_VARYING][X],
							 (int) vertex0.varyings[CG_POSITION_VARYING][Y]);

	// Push the fragment into the pipeline.
	m_cgPushFragment(fragment);
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeStupidLine(int pipelineVertexOffset)
{
	// This primitive produces many fragments from 2 vertices:
	CGVertexVaryings &vertex0 = m_pipelineVertexVaryings[pipelineVertexOffset+0];
	CGVertexVaryings &vertex1 = m_pipelineVertexVaryings[pipelineVertexOffset+1];

	// draw a line from vertex0 to vertex1
	CGVec2i from,to;

	from.set((int)vertex0.varyings[CG_POSITION_VARYING][X],
			 (int)vertex0.varyings[CG_POSITION_VARYING][Y]);

	to.set((int)vertex1.varyings[CG_POSITION_VARYING][X],
		   (int)vertex1.varyings[CG_POSITION_VARYING][Y]);

	if (from[X] > to[X]) {
		// swap from and to 
		CGVec2i tmp;
		tmp=from;
		from=to;
		to=tmp;
	}

	// Fragment to work on (initialize from vertex, set coordinates, push).
	CGFragmentData fragment;
	fragment.set(vertex0);

	// draw a line from <from> to <to>
	// Uebung 01. Aufgabe 3a)

	// Uebung01, Aufgabe 4a) (Zusatzaufgabe)
	// ...
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeLineBresenham(int pipelineVertexOffset)
{
	// ...
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeWireTriangle(int pipelineVertexOffset)
{
	// ...
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeTriangle(int pipelineVertexOffset)
{
	// ...
}
//---------------------------------------------------------------------------
