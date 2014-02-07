#ifndef CGLIGHTSOURCE_H
#define CGLIGHTSOURCE_H

#include "CG.h"
#include "CGContext.h"

class CGLightSource
{
private:
	float ambientColor[4], diffuseColor[4], specularColor[4];
	float position[4], spotDirection[4];
	float constantAttenuation,linearAttenuation,quadraticAttenuation;
	float spotExponent, spotCutoff;

public:
	/// Constructor (should default all values).
	CGLightSource(void);
	/// Destructor.
	~CGLightSource(void);
	/// Reset all values to the default (GL) light parameters.
	void setDefaultParameters();

	/// Set the ambient component color.
	void setColorAmbient (float r, float g, float b, float a);
	/// Set the diffuse component color.
	void setColorDiffuse (float r, float g, float b, float a);
	/// Set the specular component color.
	void setColorSpecular(float r, float g, float b, float a);
	/// Set the light position (float[4]).
	void setPosition		 (const float * pos);
	/// Set the spot direction (float[4]).
	void setSpotDirection(const float * spotDir);
	/// Set the spot exponent.
	void setSpotExponent (float exponent);
	/// Set the spot cutoff.
	/// Hint: store the cosine of the cutoff to avoid calculation in the vertex program
	void setSpotCutoff	 (float cutoff);
	/// Set the constant attenuation part.
	void setConstantAttenuation	 (float attenuation);
	/// Set the linear attenuation part.
	void setLinearAttenuation	 (float attenuation);
	/// Set the quadratic attenuation part.
	void setQuadraticAttenuation (float attenuation);
	/// Set all attenuation parts as float[3] array [constant,linear,quadratic].
	void setAttenuation	 (const float * attenuation);

	/// Set the uniforms in the given context to the values of the light source.
	void setupUniforms(CGContext* context);

	/// extra credit:
	/// set the uniforms for the i'th light source in context to the values of the light source
	// void setupUniforms(CGContext* context, int i);
};

#endif
