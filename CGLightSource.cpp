#include "CGLightSource.h"

//---------------------------------------------------------------------------
// Hausaufgabe 4 - Aufgabe 2  |  Alle Methoden implementiert
//---------------------------------------------------------------------------
CGLightSource::CGLightSource(void)
{
	this->setDefaultParameters();
}

CGLightSource::~CGLightSource(void)
{
}

void CGLightSource::setDefaultParameters()
{
	this->setColorAmbient(0, 0, 0, 1);
	this->setColorDiffuse(1, 1, 1, 1);
	this->setColorSpecular(1, 1, 1, 1);

	float pos[4] = { 0, 0, 1, 0 };
	this->setPosition(pos);

	float spot_dir[4] = { 0, 0, -1, 0 };
	this->setSpotDirection(spot_dir);

	this->setSpotExponent(0);
	this->setSpotCutoff(180.0F);
	this->setConstantAttenuation(1);
	this->setLinearAttenuation(0);
	this->setQuadraticAttenuation(0);
}

void CGLightSource::setColorAmbient(float r, float g, float b, float a)
{
	this->ambientColor[R] = r;
	this->ambientColor[G] = g;
	this->ambientColor[B] = b;
	this->ambientColor[A] = a;
}

void CGLightSource::setColorDiffuse(float r, float g, float b, float a)
{
	this->diffuseColor[R] = r;
	this->diffuseColor[G] = g;
	this->diffuseColor[B] = b;
	this->diffuseColor[A] = a;
}

void CGLightSource::setColorSpecular(float r, float g, float b, float a)
{
	this->specularColor[R] = r;
	this->specularColor[G] = g;
	this->specularColor[B] = b;
	this->specularColor[A] = a;
}

void CGLightSource::setPosition(const float * pos)
{
	this->position[X] = pos[X];
	this->position[Y] = pos[Y];
	this->position[Z] = pos[Z];
	this->position[W] = pos[W];
}

void CGLightSource::setSpotDirection(const float * spotDir)
{
	this->spotDirection[X] = spotDir[X];
	this->spotDirection[Y] = spotDir[Y];
	this->spotDirection[Z] = spotDir[Z];
	this->spotDirection[W] = spotDir[W];
}

void CGLightSource::setSpotExponent(float exponent)
{
	this->spotExponent = exponent;
}

void CGLightSource::setSpotCutoff(float cutoff)
{
	this->spotCutoff = cos(cutoff);
}

void CGLightSource::setConstantAttenuation(float attenuation)
{
	this->constantAttenuation = attenuation;
}

void CGLightSource::setLinearAttenuation(float attenuation)
{
	this->linearAttenuation = attenuation;
}

void CGLightSource::setQuadraticAttenuation(float attenuation)
{
	this->quadraticAttenuation = attenuation;
}

void CGLightSource::setAttenuation(const float * attenuation)
{
	this->constantAttenuation = attenuation[0];
	this->linearAttenuation = attenuation[1];
	this->quadraticAttenuation = attenuation[2];
}

void CGLightSource::setupUniforms(CGContext* context)
{
	context->cgUniform4fv(CG_ULOC_LIGHT0_AMBIENT, 1, this->ambientColor);
	context->cgUniform4fv(CG_ULOC_LIGHT0_DIFFUSE, 1, this->diffuseColor);
	context->cgUniform4fv(CG_ULOC_LIGHT0_SPECULAR, 1, this->specularColor);
	context->cgUniform4fv(CG_ULOC_LIGHT0_POSITION, 1, this->position);
	context->cgUniform4fv(CG_ULOC_LIGHT0_SPOT_DIRECTION, 1, this->spotDirection);

	context->cgUniform1fv(CG_ULOC_LIGHT0_SPOT_EXPONENT, 1, &(this->spotExponent));
	context->cgUniform1fv(CG_ULOC_LIGHT0_SPOT_CUTOFF, 1, &(this->spotCutoff));

	context->cgUniform1fv(CG_ULOC_LIGHT0_CONSTANT_ATTENUATION, 1, &(this->constantAttenuation));
	context->cgUniform1fv(CG_ULOC_LIGHT0_LINEAR_ATTENUATION, 1, &(this->linearAttenuation));
	context->cgUniform1fv(CG_ULOC_LIGHT0_QUADRATIC_ATTENUATION, 1, &(this->quadraticAttenuation));
}