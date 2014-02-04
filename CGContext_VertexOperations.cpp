#include "CGContext.h"
#include <iostream>
#include <cassert>

//---------------------------------------------------------------------------
// CGCONTEXT (VERTEX OPERATIONS)
//---------------------------------------------------------------------------
void CGContext::m_cgPullVertex(int vertexIndex)
{
	if (m_pipelineVerticesCount >= CG_MAX_VERTICES_IN_PIPELINE) {
		std::cerr << "vertex per primitive limit reached" << std::endl;  
		return;
	}
	// We copy all attributes from the attribute pointers to the vertex entry v.
	// If no pointer is given, the default value is used.
	CGVertexAttributes& v=m_pipelineVertexAttributes[m_pipelineVerticesCount++];

	if(m_pVertexAttributePointer[CG_POSITION_ATTRIBUTE]) {
		const float *pos=m_pVertexAttributePointer[CG_POSITION_ATTRIBUTE]+3*vertexIndex;
		v.attributes[CG_POSITION_ATTRIBUTE].set(pos[0], pos[1], pos[2], 1.0f);
	} else {
		v.attributes[CG_POSITION_ATTRIBUTE].set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	if(m_pVertexAttributePointer[CG_NORMAL_ATTRIBUTE]) {
		const float *nrm=m_pVertexAttributePointer[CG_NORMAL_ATTRIBUTE]+3*vertexIndex;
		v.attributes[CG_NORMAL_ATTRIBUTE].set(nrm[0], nrm[1], nrm[2], 0.0f);
	} else {
		v.attributes[CG_NORMAL_ATTRIBUTE].set(0.0f, 0.0f, 1.0f, 0.0f);
	}

	if(m_pVertexAttributePointer[CG_COLOR_ATTRIBUTE]) {
		const float *col=m_pVertexAttributePointer[CG_COLOR_ATTRIBUTE]+4*vertexIndex;
		v.attributes[CG_COLOR_ATTRIBUTE].set(col[0], col[1], col[2], col[3]);
	} else {
		v.attributes[CG_COLOR_ATTRIBUTE].set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	if(m_pVertexAttributePointer[CG_TEXCOORD_ATTRIBUTE]) {
		const float *tex=m_pVertexAttributePointer[CG_TEXCOORD_ATTRIBUTE]+2*vertexIndex;
		v.attributes[CG_TEXCOORD_ATTRIBUTE].set(tex[0], tex[1], 0.0f, 1.0f);
	} else {
		v.attributes[CG_TEXCOORD_ATTRIBUTE].set(0.0f, 0.0f, 0.0f, 1.0f);
	}
}
//---------------------------------------------------------------------------
void CGContext::m_cgVertexPipeline()
{
	// Do first part of vertex processing right here.
	m_cgVertexProgram();
  
	// Process this vertex for the current primitive.
	m_cgPrimitiveProcessing();

	// Note: rest of the 'original' vertex pipeline is finished after
	// the primitive processing because new vertices might have been
	// created after clipping.
}
//---------------------------------------------------------------------------
void CGContext::m_cgVertexProgram()
{
	// Run programmable vertex processing.
	m_vertexProgram(m_pipelineVertexAttributes[m_pipelineVerticesCount-1], 
					m_pipelineVertexVaryings[m_pipelineVerticesCount-1], 
					m_uniforms);
}
//---------------------------------------------------------------------------
void CGContext::m_cgPrimitiveProcessing()
{
	// Primitive assembly stage: collect vertices in m_pipelineVertexVaryings
	// until enough entries for the current primitive are present.
	int verticesNeeded = 0;
	switch(m_primitiveMode) {
		case CG_POINTS:     verticesNeeded = 1; break;
		case CG_LINES:      verticesNeeded = 2; break;
		case CG_TRIANGLES:  verticesNeeded = 3; break;
		// Insert other primitive types here.
	}
	if (m_pipelineVerticesCount < verticesNeeded)
		return;
	// else m_pipelineVerticesCount == verticesNeeded and we can process this primitive

	// This might split our one primitive and create new ones.
	// This is deactivated initally. Later, we will enable this:
	// int pipelinePrimitiveCount = m_cgClipPrimitive();
	// But for now, just use the input primitive.
	int pipelinePrimitiveCount = 1;

	// Finish the 'original' vertex pipeline for all pipeline vertices.
	for(int i=0; i<m_pipelineVerticesCount; i++) {
		m_cgVertexPerspectiveDivide(i);
		m_cgVertexViewportTransform(i);
	}

	// We might have to rasterize more than one primitive.
	for(int i=0; i<pipelinePrimitiveCount; i++) {

		m_cgRasterize(i);
	}

	// Purge vertex list.
	m_pipelineVerticesCount = 0;
}

//---------------------------------------------------------------------------
// Koordinaten werden vom Clip-Space in normalisierte Geräte-Koordinaten
// überführt
//
// Übung 07 - Aufgabe 2d  |  Umwandlung in normalisierte Geräte-Koordinaten
//---------------------------------------------------------------------------
void CGContext::m_cgVertexPerspectiveDivide(int pipelineVertex)
{
	// We are interested in the last vertex (as always if no vertex index available).
	CGVertexVaryings& v=m_pipelineVertexVaryings[pipelineVertex];
	CGVec4 &pos = v.varyings[CG_POSITION_VARYING];

	// Transform from Clip Space To Normalized Device Coordinates
	pos[3] = 1 / pos[3];
	pos[0] = pos[0] * pos[3];
	pos[1] = pos[1] * pos[3];
	pos[2] = pos[2] * pos[3];
}

//---------------------------------------------------------------------------
// Koordinaten werden von normalisierte Geräte-Koordinaten in Window-Space
// überführt, aber nur in einem bestimmten Bereich (viewport)
//
// Übung 07 - Aufgabe 2e  |  Umwandlung in Windows-Space
// Übung 07 - Aufgabe 3a  |  ViewPort wird berücksichtigt
//---------------------------------------------------------------------------
void CGContext::m_cgVertexViewportTransform(int pipelineVertex)
{
	// We are interested in the last vertex (as always if no vertex index available).
	CGVertexVaryings& v=m_pipelineVertexVaryings[pipelineVertex];
	CGVec4 &pos = v.varyings[CG_POSITION_VARYING];

	// Transform from Normalized Device Coordinates To Window Space
	pos[0] = ((pos[0] + 1) * m_viewport[2] / 2) + m_viewport[0];
	pos[1] = ((pos[1] + 1) * m_viewport[3] / 2) + m_viewport[1];
}
