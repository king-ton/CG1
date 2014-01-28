#ifndef CG_CONTEXT_H
#define CG_CONTEXT_H

#include "CG.h"            // This is the internal global header.
#include "CGFrameBuffer.h" // Our context needs a framebuffer.
#include "CGProgramInterface.h"
#include "CGMatrix.h"
#include "CGTexture.h"

#include <math.h>
#include <string.h>

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 1a  |  Funktion cgReadPixels registriert
//---------------------------------------------------------------------------


class CGContext 
{
	// Attributes.
private:

	/// Framebuffer.
	CGFrameBuffer m_frameBuffer;

	/// Internal state: vertex attribute pointers.
	float *m_pVertexAttributePointer[CG_ATTRIBUTE_COUNT];
  
	/// Internal state: current primitive mode.
	int m_primitiveMode;

	/// Internal state: current polygon rasterization mode.
	int m_polygonMode;

	/// Internal state: clear color.
	CGVec4 m_clearColor;

	/// Internal state: vertex data still in the pipeline (not yet enough for the respective primitive type).
	CGVertexAttributes m_pipelineVertexAttributes[CG_MAX_VERTICES_IN_PIPELINE];
	CGVertexVaryings m_pipelineVertexVaryings[CG_MAX_VERTICES_IN_PIPELINE];

	/// Internal state: number of vertices still hanging in the pipeline (for rasterizer).
	int m_pipelineVerticesCount;

	/// Internal state: fragments waiting to be processed.
	CGFragmentData m_pipelineFragments[CG_MAX_FRAGMENTS_IN_PIPELINE];

	/// Internal state: number of fragments waiting to be processed.
	int m_pipelineFragmentsCount;

	/// Internal state: capabilities.
	struct
	{
		bool blend, depthTest, cullFace, useBresenham;
	} m_capabilities;

	/// Internal state: viewport
	int m_viewport[4];

	/// Internal state: textures
	struct
	{
		int textureUnits[CG_MAX_TEXTURE_UNITS];
		CGTexture2D *texturePool;
		int texturePoolSize;
	} m_textures;

	/// Internal state: uniform variables.
	CGUniformData m_uniforms;

	/// Internal state: shader programs
	CGVertexProgram m_vertexProgram;
	CGFragmentProgram m_fragmentProgram;

	// Methods.
public:
	/// Context constructor.
	CGContext(int width, int height);

	/// Context destructor.
	~CGContext();

	/// Provide specific vertex attribute. Supported: CG_POSITION, CG_NORMAL, CG_COLOR, CG_TEXCOORD.
	void cgVertexAttribPointer(CGenum attribute, float *data);

	/// Clears the bitwise specified buffers by the current clear value (e.g. m_cgClearColor)
	void cgClearColor(float red, float green, float blue, float alpha);

	/// Clears the bitwise specified buffers by the current clear value (e.g. m_cgClearColor)
	void cgClear(int bitfield);

	/// Render primitives with given mode. Note: this calls the pipeline directly (in our implementation).
	void cgDrawArrays(CGenum mode, int first, int count);

	/// Enables capability. Supported: CG_CULL_FACE, CG_LIGHTING, Cg_DEPTH_TEST, CG_BLEND.
	void cgEnable(CGenum cap);

	/// Disables capability. Supported: CG_CULL_FACE, CG_LIGHTING, Cg_DEPTH_TEST, CG_BLEND.
	void cgDisable(CGenum cap);

	/// Sets polygon mode, only supports CG_LINE and CG_FILL for BOTH faces.
	void cgPolygonMode(CGenum mode);

	/// Set the shader programs.
	void cgUseProgram(CGVertexProgram vertexProgram, CGFragmentProgram fragmentProgram);

	/// Set uniform int variable.
	void cgUniform1i(int location, int value);

	/// Set uniform float variable. Pass count 1 for single float, >1 for multiple.
	void cgUniform1fv(int location, int count, const float* value);

	/// Set uniform 4f variable. Pass count 1 for single 4f variable, >1 for multiple.
	void cgUniform4fv(int location, int count, const float* value);

	/// Set uniform matrix 4x4f variable. Pass count 1 for single matrix.
	/// Transpose when passing row-major.
	void cgUniformMatrix4fv(int location, int count, bool transpose, const float* value);

	/// Set the viewport
	void cgViewport(int x, int y, int width, int height);

	/// Generate and upload a texture image (these are two steps in OpenGL).
	int cgGenTexture_cgBindTexture_cgTexImage2D(int width, int height, unsigned char* data4ub);

	/// Activate a texture unit and bind a named texture (these are two steps in OpenGL).
	void cgActiveTexture_cgBindTexture_cgTexParameter(int textureUnit, int textureName, int filter, int wrap);

	/// Reads all data from the current color buffer into the provided memory.
	void cgReadPixels(unsigned char* data);

private:
	// Pipeline methods

	/// Validate current state and prepare pipeline for rendering.
	bool m_cgValidateState();


	// Pipeline methods (_VertexOperations.cpp).

	/// Initialize pipeline vertex with given (or default) attributes.
	void m_cgPullVertex(int vertexIndex);

	/// Execute pipeline components on current vertex data.
	void m_cgVertexPipeline();

	/// Execute vertex shader on current vertex.
	void m_cgVertexProgram();

	/// Process primitive (clipping and finishing vertex pipeline) if enough vertices piped.
	void m_cgPrimitiveProcessing();

	/// Transform Clip Space to Normalized Device Coordinates (NDC)
	void m_cgVertexPerspectiveDivide(int pipelineVertex);

	/// Transform Normalized Device Coordinates to Window Space
	void m_cgVertexViewportTransform(int pipelineVertex);


	// Pipeline methods (_PrimitiveOperations.cpp).

	/// Clip primitive (in clip space) and create new primitives if needed. Returns number of resulting primitives.
	int m_cgClipPrimitive();
  
	/// Test if triangle should be culled. Returns true if culled.
	bool m_cgBFCullTriangle(CGVec4 &vertex0Position, CGVec4 &vertex1Position, CGVec4 &vertex2Position);

	/// Rasterize primitive and execute pipeline fragment part.
	void m_cgRasterize(int primitiveNumber);

	/// Simple point rasterizer.
	void m_cgRasterizePoint(int pipelineVertexOffset);
  
	/// Stupid line rasterizer.
	void m_cgRasterizeStupidLine(int pipelineVertexOffset);

	/// Bresenham rasterizer.
	void m_cgRasterizeLineBresenham(int pipelineVertexOffset);

	/// Wireframe triangle rasterizer.
	void m_cgRasterizeWireTriangle(int pipelineVertexOffset);

	/// Triangle rasterizer.
	void m_cgRasterizeTriangle(int pipelineVertexOffset);

	/// Line rasterizer for custom offsets.
	void m_cgRasterizeLine(int pipelineVertexOffset1, int pipelineVertexOffset2);


	// Pipeline methods (_FragmentOperations.cpp).

	/// Enqueue fragment for processing
	void m_cgPushFragment(CGFragmentData& fragment);

	/// Process all fragments currently in the pipeline.
	void m_cgFlushFragments();

	/// Execute pipeline components on fragment data.
	void m_cgFragmentPipeline(CGFragmentData& fragment);

	/// Test current fragment coordinates against framebuffer. Returns false if to be clipped.
	bool m_cgFragmentClipping(CGFragmentData& fragment);

	/// Execute fragment shader on fragment
	void m_cgFragmentProgram(CGFragmentData& fragment);

	/// Test current fragment against depth buffer and update. Returns false if test fails.
	bool m_cgFragmentZTest(CGFragmentData& fragment);

	/// Blend the fragment color from the underlying framebuffer color. Returns true.
	bool m_cgFragmentBlending(CGFragmentData& fragment);

	/// Write current fragment into fragment buffer.
	void m_cgFragmentWriteBuffer(CGFragmentData& fragment);
};

//---------------------------------------------------------------------------

#endif

