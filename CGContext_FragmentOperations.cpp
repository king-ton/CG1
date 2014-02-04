#include "CGContext.h"
#include <iostream>
#include <cassert>

//---------------------------------------------------------------------------
// CGCONTEXT (FRAGMENT OPERATIONS)
//---------------------------------------------------------------------------
void CGContext::m_cgPushFragment(CGFragmentData& fragment)
{
	// Wait for enough fragments to process.
	m_pipelineFragments[m_pipelineFragmentsCount++] = fragment;
	if(m_pipelineFragmentsCount == CG_MAX_FRAGMENTS_IN_PIPELINE)
		m_cgFlushFragments();
}
//---------------------------------------------------------------------------
void CGContext::m_cgFlushFragments()
{
	// Run fragment pipeline components for each fragment.
	// Uncomment following line for parallel fragment processing (if OpenMP is activated)

	//#pragma omp parallel for
	for(int i=0; i<m_pipelineFragmentsCount; ++i)
		m_cgFragmentPipeline(m_pipelineFragments[i]);

	// All fragments processed, clear pipeline.
	m_pipelineFragmentsCount = 0;
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentPipeline(CGFragmentData& fragment)
{
	if(!m_cgFragmentClipping(fragment))
		return;

	m_cgFragmentProgram(fragment);

	if(m_capabilities.depthTest && !m_cgFragmentZTest(fragment))
		return;

	if(m_capabilities.blend && !m_cgFragmentBlending(fragment)) 
		return;

	m_cgFragmentWriteBuffer(fragment);
}

//---------------------------------------------------------------------------
// Fragmente außerhalb des Bereiches (viewport) und |z| > 1 werden verworfen
//
// Übung 03 - Aufgabe 3a  |  Fragmente außerhalb des Framebuffers werden
//						  |  verworfen
// Übung 05 - Aufgabe 3a  |  Fragmente außerhalb des Z-Bereichs werden
//						  |  verworfen
// Übung 07 - Aufgabe 3b  |  ViewPort wird berücksichtigt
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentClipping(CGFragmentData& fragment)
{
	if (fragment.coordinates[X] >= m_viewport[0] + m_viewport[2] ||
		fragment.coordinates[Y] >= m_viewport[1] + m_viewport[3] ||
		fragment.coordinates[X] < m_viewport[0] ||
		fragment.coordinates[Y] < m_viewport[1] ||
		fragment.varyings[CG_POSITION_VARYING][Z] < -1 ||
		fragment.varyings[CG_POSITION_VARYING][Z] > 1)
		return false;
	return true;
}

//---------------------------------------------------------------------------
void CGContext::m_cgFragmentProgram(CGFragmentData& fragment)
{
	// Run programmable fragment processing.
	CGVec4 color;
	m_fragmentProgram(fragment, color, m_uniforms);
	fragment.varyings[CG_COLOR_VARYING] = color;
}

//---------------------------------------------------------------------------
// Überprüft, ob das aktuelle Fragment hinter dem Fragment aus dem
// FrameBuffer liegt
//
// Übung 05 - Aufgabe 3d  |  Prüfen, ob Pixel dahinter liegt
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentZTest(CGFragmentData& fragment)
{
	static const float depthTolerance = 1e-6f;
	if (m_frameBuffer.depthBuffer.get(fragment.coordinates[X], fragment.coordinates[Y]) + depthTolerance > fragment.varyings[CG_POSITION_VARYING][Z])
		return true;
	else
		return false;
}

//---------------------------------------------------------------------------
// Übung 06 - Aufgabe 2a  |  Methode implementiert
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentBlending(CGFragmentData& fragment)
{
	float rgba[4];
	m_frameBuffer.colorBuffer.get(fragment.coordinates[X], fragment.coordinates[Y], rgba);

	float* color = fragment.varyings[CG_COLOR_VARYING].elements;
	for (int i = 0; i < 3; ++i)
		color[i] = color[i] * color[3] + rgba[i] * (1 - color[3]);

	return true;
}

//---------------------------------------------------------------------------
// Übung 05 - Aufgabe 3d  |  Tiefe setzen, falls Fragment davor
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentWriteBuffer(CGFragmentData& fragment)
{
	// Write the current fragment into the framebuffer.
	// color into color buffer
	m_frameBuffer.colorBuffer.set(fragment.coordinates[X],
								  fragment.coordinates[Y],
								  fragment.varyings[CG_COLOR_VARYING].elements);

	// Schreibe Tiefe in Tiefen-Buffer
	if (m_cgFragmentZTest(fragment))
		m_frameBuffer.depthBuffer.set(	fragment.coordinates[X],
										fragment.coordinates[Y],
										fragment.varyings[CG_POSITION_VARYING][Z]);
}
//---------------------------------------------------------------------------
