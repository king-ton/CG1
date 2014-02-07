#ifndef CG_PROGRAMINTERFACE_H
#define CG_PROGRAMINTERFACE_H

#include "CG.h"
#include "CGMatrix.h"
#include "CGTexture.h"

//---------------------------------------------------------------------------
// Hausaufgabe 4 - Aufgabe 1.1a  |  Hinzufügen neuer Locations
//---------------------------------------------------------------------------

/// Locations to access the builtin Uniforms
enum CGUniformLocations
{
	CG_ULOC_PROJECTION_MATRIX=0,
	CG_ULOC_MODELVIEW_MATRIX,
	CG_ULOC_NORMAL_MATRIX,
	CG_ULOC_MATERIAL_AMBIENT,
	CG_ULOC_MATERIAL_DIFFUSE,
	CG_ULOC_MATERIAL_SPECULAR,
	CG_ULOC_MATERIAL_SHININESS,
	CG_ULOC_MATERIAL_EMISSION,
	CG_ULOC_SCENE_AMBIENT,
	CG_ULOC_LIGHT0_AMBIENT,
	CG_ULOC_LIGHT0_DIFFUSE,
	CG_ULOC_LIGHT0_SPECULAR,
	CG_ULOC_LIGHT0_POSITION,
	CG_ULOC_LIGHT0_CONSTANT_ATTENUATION,
	CG_ULOC_LIGHT0_LINEAR_ATTENUATION,
	CG_ULOC_LIGHT0_QUADRATIC_ATTENUATION,
	CG_ULOC_LIGHT0_SPOT_DIRECTION,
	CG_ULOC_LIGHT0_SPOT_EXPONENT,
	CG_ULOC_LIGHT0_SPOT_CUTOFF,
	CG_ULOC_SAMPLER
};

#define CG_ATTRIBUTE_COUNT 4

/// Access offsets into attribute data arrays.
enum CGAttributes
{
	CG_POSITION_ATTRIBUTE=0, 
	CG_NORMAL_ATTRIBUTE, 
	CG_COLOR_ATTRIBUTE, 
	CG_TEXCOORD_ATTRIBUTE
};

#define CG_VARYING_COUNT 5

/// Access offsets into varying data arrays.
enum CGVaryings
{
	CG_POSITION_VARYING=0, 
	CG_NORMAL_VARYING, 
	CG_COLOR_VARYING, 
	CG_TEXCOORD_VARYING,
	CG_POSITION_EYESPACE_VARYING
};

/// Vertex attribute container (vertex processing inputs).
struct CGVertexAttributes
{
	CGVec4 attributes[CG_ATTRIBUTE_COUNT];  // Attributes are what is pulled from the attribute pointer.
};

/// Vertex varying container (vertex processing outputs).
struct CGVertexVaryings
{
	CGVec4 varyings[CG_VARYING_COUNT];      // Varyings are vertex attributes (and more) after vertex program.
};

/// Fragment data container.
struct CGFragmentData
{
	CGVec2i coordinates;                  // Fragment specific framebuffer coordinate.
	CGVec4  varyings[CG_VARYING_COUNT];   // Varyings as copied from vertex or interpolated.
	void set(const CGVertexVaryings& vertex)
	{
		for (int i=0; i<CG_VARYING_COUNT; i++)
			varyings[i].set(vertex.varyings[i]);
	}

	//---------------------------------------------------------------------------
	// Übung 03 - Aufgabe 1b  |  Interpolation implementiert
	// Übung 11 - Aufgabe 3a  |  Perspektivisch korrekte Interpolation
	//---------------------------------------------------------------------------
	void set(const CGVertexVaryings& A, const CGVertexVaryings& B, float ratio)
	{
		float wa = A.varyings[CG_POSITION_VARYING][W];
		float wb = B.varyings[CG_POSITION_VARYING][W];

		float a = (1.0F - ratio), b = ratio;

		for (int i = 0; i < CG_VARYING_COUNT; ++i)
			for (int j = 0; j < 4; ++j)
				varyings[i][j] = (a * wa * A.varyings[i][j] + b * wb * B.varyings[i][j]) / (a * wa + b * wb);
	}

	//---------------------------------------------------------------------------
	// Übung 04 - Aufgabe 2a  |  Interpolation implementiert
	// Übung 11 - Aufgabe 3a  |  Perspektivisch korrekte Interpolation
	//---------------------------------------------------------------------------
	void set(const CGVertexVaryings& A, const CGVertexVaryings& B, const CGVertexVaryings& C, float a, float b, float c)
	{
		float wa = A.varyings[CG_POSITION_VARYING][W];
		float wb = B.varyings[CG_POSITION_VARYING][W];
		float wc = C.varyings[CG_POSITION_VARYING][W];

		for (int i = 0; i < CG_VARYING_COUNT; ++i)
			for (int j = 0; j < 4; ++j) 
				varyings[i][j] = (a * wa * A.varyings[i][j] + b * wb * B.varyings[i][j] + c * wc * C.varyings[i][j]) / (a * wa + b * wb + c * wc);
	}
};

/// Texture sampler.
struct CGSampler2D
{
	int unit;
	const CGTexture2D *texture;
};

//---------------------------------------------------------------------------
// Uniform variables.
//
// Hausaufgabe 4 - Aufgabe 1.1b  |  Variablen hinzugefügt
//---------------------------------------------------------------------------
struct CGUniformData
{
	CGMatrix4x4 projectionMatrix, modelviewMatrix, normalMatrix;
	CGVec4 materialAmbient, materialDiffuse, materialSpecular, materialEmission;
	float materialShininess, light0SpotExponent, light0SpotCutoff, light0ConstantAttenuation,
		light0LinearAttenuation, light0QuadraticAttenuation;
	CGVec4 sceneAmbient, light0Ambient, light0Diffuse, light0Specular, light0Position, light0SpotDirection;
	CGSampler2D sampler;
};

/// Shader functions
typedef void (*CGVertexProgram)(const CGVertexAttributes&, CGVertexVaryings&, const CGUniformData&);
typedef void (*CGFragmentProgram)(const CGFragmentData&, CGVec4&, const CGUniformData&);

#endif
