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
	switch (m_primitiveMode) {
	case CG_POINTS:
		m_pipelineVerticesCount =
			CGPrimClipper::clipPoint(m_pipelineVertexVaryings, m_pipelineVerticesCount);
		newPrimCount = m_pipelineVerticesCount;
		break;
	case CG_LINES:
		m_pipelineVerticesCount =
			CGPrimClipper::clipLine(m_pipelineVertexVaryings, m_pipelineVerticesCount);
		newPrimCount = m_pipelineVerticesCount / 2;
		break;
	case CG_TRIANGLES:
		// Clipped vertices need to be re-assembled into triangles
		newPrimCount = CGPrimClipper::clipPoly(m_pipelineVertexVaryings, m_pipelineVerticesCount) - 2;
		m_pipelineVerticesCount = 0; //reset the pipeline
		for (int i = 0; i < newPrimCount; ++i) { //as triangle fan
			m_pipelineVertexVaryings[m_pipelineVerticesCount++] = CGPrimClipper::outBuf()[0];
			m_pipelineVertexVaryings[m_pipelineVerticesCount++] = CGPrimClipper::outBuf()[i + 1];
			m_pipelineVertexVaryings[m_pipelineVerticesCount++] = CGPrimClipper::outBuf()[i + 2];
		}
		break;
		// Insert other primitive types here.
	}

	// Returns the new primitive count
	return newPrimCount;
}

//---------------------------------------------------------------------------
// Übung 5 - Aufgabe 2b  |  Rückseiten von Primitiven werden nicht gezeichnet
//---------------------------------------------------------------------------
bool CGContext::m_cgBFCullTriangle(CGVec4 &vertex0Position,
	CGVec4 &vertex1Position,
	CGVec4 &vertex2Position)
{
	float ax = vertex0Position.elements[0];
	float ay = vertex0Position.elements[1];
	float bx = vertex1Position.elements[0];
	float by = vertex1Position.elements[1];
	float cx = vertex2Position.elements[0];
	float cy = vertex2Position.elements[1];

	if (((bx - ax) * (cy - ay) - (by - ay) * (cx - ax)) >= 0)
		return false;
	else
		return true;
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

	switch (m_primitiveMode) {
	case CG_POINTS:
		m_cgRasterizePoint(primitiveNumber * 1);
		break;
	case CG_LINES:
		if (m_capabilities.useBresenham)
			m_cgRasterizeLineBresenham(primitiveNumber * 2);
		else
			m_cgRasterizeStupidLine(primitiveNumber * 2);
		break;
	case CG_TRIANGLES:
		if (m_polygonMode == CG_LINE)
			m_cgRasterizeWireTriangle(primitiveNumber * 3);
		else
			m_cgRasterizeTriangle(primitiveNumber * 3);
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
	CGVertexVaryings &vertex0 = m_pipelineVertexVaryings[pipelineVertexOffset + 0];

	// Initialize the fragment.
	CGFragmentData fragment;
	fragment.set(vertex0);

	// And set coordinates. (SetFragment initializes just the attributes + varyings).
	fragment.coordinates.set((int)vertex0.varyings[CG_POSITION_VARYING][X],
		(int)vertex0.varyings[CG_POSITION_VARYING][Y]);

	// Push the fragment into the pipeline.
	m_cgPushFragment(fragment);
}

//---------------------------------------------------------------------------
// Übung 01 - Aufgabe 3a  |  Erzeugen einer Linie zwischen 2 Vertices
// Übung 02 - Aufgabe 1b  |  Austausch gegen bessere Variante
// Übung 03 - Aufgabe 2a  |  Refaktorisierung
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeStupidLine(int pipelineVertexOffset)
{
	m_cgRasterizeLine(pipelineVertexOffset, pipelineVertexOffset + 1);
}

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 2a  |  Refaktorisierung
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeLineBresenham(int pipelineVertexOffset)
{
	m_cgRasterizeLine(pipelineVertexOffset, pipelineVertexOffset + 1);
}

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 2a  |  Funktion implementiert
// Übung 05 - Aufgabe 2b  |  Rückseiten von Primitiven werden nicht gezeichnet
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeWireTriangle(int pipelineVertexOffset)
{
	CGVertexVaryings *vertex = m_pipelineVertexVaryings + pipelineVertexOffset;

	CGVec4 &vertex0 = vertex[0].varyings[CG_POSITION_VARYING];
	CGVec4 &vertex1 = vertex[1].varyings[CG_POSITION_VARYING];
	CGVec4 &vertex2 = vertex[2].varyings[CG_POSITION_VARYING];

	if (m_cgBFCullTriangle(vertex0, vertex1, vertex2) == false) {
		for (int i = 0; i < 3; i++) {
			m_cgRasterizeLine(pipelineVertexOffset + i, pipelineVertexOffset + (i + 1) % 3);
		}
	}
}

//---------------------------------------------------------------------------
// Übung 04 - Aufgabe 1a  |  Funktion implementiert
// Übung 04 - Aufgabe 2b  |  Interpolation hinzugefügt
// Übung 04 - Aufgabe 3   |  Optimierung
// Übung 05 - Aufgabe 2b  |  Rückseiten von Primitiven werden nicht gezeichnet
// Übung 05 - Aufgabe 2c  |  Optimierung Backface-Culling
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeTriangle(int pipelineVertexOffset)
{
	CGVertexVaryings *vertex = m_pipelineVertexVaryings + pipelineVertexOffset;
	CGVertexVaryings &A = vertex[0], &B = vertex[1], &C = vertex[2];

	CGVec4 &vertex0 = A.varyings[CG_POSITION_VARYING];
	CGVec4 &vertex1 = B.varyings[CG_POSITION_VARYING];
	CGVec4 &vertex2 = C.varyings[CG_POSITION_VARYING];

	float det = (vertex1[X] - vertex0[X]) * (vertex2[Y] - vertex0[Y]) -
				(vertex1[Y] - vertex0[Y]) * (vertex2[X] - vertex0[X]);

	if (det >= 0) {
		CGFragmentData fragment;

		int minX = m_frameBuffer.getWidth(),
			minY = m_frameBuffer.getHeight(),
			maxX = -1,
			maxY = -1;

		for (int i = 0; i < 3; ++i) {
			int x = vertex[i].varyings[CG_POSITION_VARYING][X],
				y = vertex[i].varyings[CG_POSITION_VARYING][Y];

			if (x < minX) minX = x;
			if (x > maxX) maxX = x;
			if (y < minY) minY = y;
			if (y > maxY) maxY = y;
		}


		for (int iY = minY; iY <= maxY; iY++)
		for (int iX = minX; iX <= maxX; iX++) {
			float x = iX + 0.5F, y = iY + 0.5F;

			float x0 = vertex0[X] - x;
			float y0 = vertex0[Y] - y;
			float x1 = vertex1[X] - x;
			float y1 = vertex1[Y] - y;
			float x2 = vertex2[X] - x;
			float y2 = vertex2[Y] - y;

			float a = (x1 * y2) - (x2 * y1);
			float b = (x2 * y0) - (x0 * y2);
			float c = (x0 * y1) - (x1 * y0);

			if (a >= 0 & b >= 0 & c >= 0) {
				fragment.coordinates.set(iX, iY);
				fragment.set(A, B, C, a / det, b / det, c / det);
				m_cgPushFragment(fragment);
			}
		}
	}
}

//---------------------------------------------------------------------------
// Übung 02 - Aufgabe 1b  |  Aus Vorgabe eingefügt
// Übung 02 - Aufgabe 2   |  Bresenham implementiert für 0<=m<=1
// Übung 02 - Aufgabe 3   |  Bresenham erweitert für beliebigen Anstieg
// Übung 03 - Aufgabe 1c  |  Interpolation hinzugefügt
// Übung 03 - Aufgabe 2a  |  Refaktorisierung
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeLine(int pipelineVertexOffset1, int pipelineVertexOffset2)
{
	// This primitive produces many fragments from 2 vertices:
	CGVertexVaryings vertex0 = m_pipelineVertexVaryings[pipelineVertexOffset1];
	CGVertexVaryings vertex1 = m_pipelineVertexVaryings[pipelineVertexOffset2];

	// draw a line from vertex0 to vertex1
	CGVec2i from, to;
	from.set((int)vertex0.varyings[CG_POSITION_VARYING][X], (int)vertex0.varyings[CG_POSITION_VARYING][Y]);
	to.set((int)vertex1.varyings[CG_POSITION_VARYING][X], (int)vertex1.varyings[CG_POSITION_VARYING][Y]);

	// fragment to work on (initialize from vertex, set coordinates, push).
	CGFragmentData fragment;

	///---------------------------------------------------------------------------
	/// Übung 02 - Aufgabe 3   |  Tausche x und y-Werte, falls Anstieg m > |1|,
	///						   |  Startpunkt muss links vom Endpunkt liegen
	///---------------------------------------------------------------------------
	bool swapxy = abs(to[Y] - from[Y]) > abs(to[X] - from[X]);

	if (swapxy) {
		int tmp;
		tmp = to[Y];	to[Y] = to[X];		to[X] = tmp;
		tmp = from[Y];	from[Y] = from[X];	from[X] = tmp;
	}

	// Startpunkt liegt weiter links als Endpunkt?
	if (from[X] > to[X]) {
		int tmp;
		tmp = from[X]; from[X] = to[X]; to[X] = tmp;
		tmp = from[Y]; from[Y] = to[Y]; to[Y] = tmp;

		CGVertexVaryings vertexTmp = vertex0;
		vertex0 = vertex1; vertex1 = vertexTmp;
	}

	///---------------------------------------------------------------------------
	/// Übung 02 - Aufgabe 2   |  Bresenham implementiert für 0<=m<=1
	/// Übung 03 - Aufgabe 1c  |  Interpolation hinzugefügt
	///---------------------------------------------------------------------------
	int x = from[X], y = from[Y];
	int dx = to[X] - from[X];
	int dy = abs(to[Y] - from[Y]);

	int deltaE = 2 * dy, deltaNE = 2 * (dy - dx);
	int d = 2 * dy - dx;

	int ystep = (from[Y] < to[Y]) ? 1 : -1;
	float ratio = 0.0F;
	float xstep = 1.0F / dx;

	while (x <= to[X]) {

		if (swapxy)
			fragment.coordinates.set(y, x);
		else
			fragment.coordinates.set(x, y);

		if (d < 0)
			d += deltaE;
		else {
			y += ystep;
			d += deltaNE;
		}

		fragment.set(vertex0, vertex1, ratio);
		m_cgPushFragment(fragment);

		ratio += xstep;
		++x;
	}
}
