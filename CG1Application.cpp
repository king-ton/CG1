// Welche Übung soll ausgeführt werden?
#define U12

// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>           // for time() in srand()

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 2c   |  CGImageFile hinzugefügt
// Übung 09      - Aufgabe 1    |  CGMath und CG1Application_renderSphere
//							    |  hinzugefügt
// Hausaufgabe 3 - Aufgabe 1.2  |  CGQuadric hinzugefügt
// Übung 12      - Aufgabe 1a   |  TestDataSet hinzugefügt
//---------------------------------------------------------------------------
// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"
#include "CGImageFile.h"
#include "CGMath.h"
#include "CG1Application_renderSphere.h"
#include "CGQuadric.h"
#include "TestDataSet.h"

//---------------------------------------------------------------------------
// GLOBALE VARIABLEN
//---------------------------------------------------------------------------
CGContext *ourContext;

//---------------------------------------------------------------------------
// VERTEX PROGRAMME
//---------------------------------------------------------------------------
#if defined(U1) || defined(U2) || defined(U3_1) || defined(U3_2) || defined(U3_3) || defined(U4) || defined(U5) || defined(U6) || defined(U6_4) || defined(HA1)
//---------------------------------------------------------------------------
// generic "passthorugh" vertex program
void passthroughVertexProgram(	const CGVertexAttributes& in,
								CGVertexVaryings& out,
								const CGUniformData& uniforms)
{
	out.varyings[CG_POSITION_VARYING] = in.attributes[CG_POSITION_ATTRIBUTE];
	out.varyings[CG_NORMAL_VARYING] = in.attributes[CG_NORMAL_ATTRIBUTE];
	out.varyings[CG_COLOR_VARYING] = in.attributes[CG_COLOR_ATTRIBUTE];
	out.varyings[CG_TEXCOORD_VARYING] = in.attributes[CG_TEXCOORD_ATTRIBUTE];
}
#endif

//---------------------------------------------------------------------------
// Übung 07 - Aufgabe 2b  |  Vertex-Programm erstellt, Transformation der
//							 Vertex-Position mithilfe der Projektions-Matrix
//---------------------------------------------------------------------------
#if defined(U7)
void projectionVertexProgram(	const CGVertexAttributes& in,
								CGVertexVaryings& out,
								const CGUniformData& uniforms)
{
	out.varyings[CG_POSITION_VARYING] = uniforms.projectionMatrix * in.attributes[CG_POSITION_ATTRIBUTE];

	out.varyings[CG_NORMAL_VARYING] = in.attributes[CG_NORMAL_ATTRIBUTE];
	out.varyings[CG_COLOR_VARYING] = in.attributes[CG_COLOR_ATTRIBUTE];
	out.varyings[CG_TEXCOORD_VARYING] = in.attributes[CG_TEXCOORD_ATTRIBUTE];
}
#endif

//---------------------------------------------------------------------------
// Übung 08 - Aufgabe 2a  |  Vertex-Programm erstellt, Transformation der
//						  |	 Vertex-Position mithilfe der ModelView-Matrix
//						  |  und der Projections-Matrix
//---------------------------------------------------------------------------
#if defined(U8) || defined(U12) || defined(HA3)
void modelViewProjectionVertexProgram(const CGVertexAttributes& in,
	CGVertexVaryings& out,
	const CGUniformData& uniforms)
{
	out.varyings[CG_POSITION_VARYING] = uniforms.projectionMatrix * uniforms.modelviewMatrix * in.attributes[CG_POSITION_ATTRIBUTE];

	out.varyings[CG_NORMAL_VARYING] = in.attributes[CG_NORMAL_ATTRIBUTE];
	out.varyings[CG_COLOR_VARYING] = in.attributes[CG_COLOR_ATTRIBUTE];
	out.varyings[CG_TEXCOORD_VARYING] = in.attributes[CG_TEXCOORD_ATTRIBUTE];
}
#endif

//---------------------------------------------------------------------------
// Übung 09 - Aufgabe 2   |  Emmisiver, ambienter und diffuser Anteil
//						  |  implementiert
// Übung 09 - Aufgabe 3   |  Spekularer Anteil implementiert
//---------------------------------------------------------------------------
#if defined(U9) || defined(U10) || defined(U12)
void perVertexLighingVertexProgram(	const CGVertexAttributes& in,
									CGVertexVaryings& out,
									const CGUniformData& uniforms)
{
	// Get hold of all vertex attributes.
	CGVec4 aPos = in.attributes[CG_POSITION_ATTRIBUTE];
	CGVec4 aNrm = in.attributes[CG_NORMAL_ATTRIBUTE];
	CGVec4 aClr = in.attributes[CG_COLOR_ATTRIBUTE];
	CGVec4 aTex = in.attributes[CG_TEXCOORD_ATTRIBUTE];

	// Get hold of all vertex varyings.
	CGVec4 &vPos = out.varyings[CG_POSITION_VARYING];
	CGVec4 &vNrm = out.varyings[CG_NORMAL_VARYING];
	CGVec4 &vClr = out.varyings[CG_COLOR_VARYING];
	CGVec4 &vTex = out.varyings[CG_TEXCOORD_VARYING];

	// Default program copies all attributes into all varyings used:
	vPos = aPos; vNrm = aNrm; vClr = aClr; vTex = aTex;

	// Transform from Object Space into Eye Space.
	vPos = uniforms.modelviewMatrix * vPos;
	vNrm = CGMath::normalize(uniforms.normalMatrix * vNrm);

	CGVec4 emis, ambi, diff, spec;
	// TODO: for now, set them all to 0
	emis.set(0.0f, 0.0f, 0.0f, 0.0f); ambi.set(0.0f, 0.0f, 0.0f, 0.0f);
	diff.set(0.0f, 0.0f, 0.0f, 0.0f); spec.set(0.0f, 0.0f, 0.0f, 0.0f);

	emis = uniforms.materialEmission;
	ambi = CGMath::mul(uniforms.materialAmbient, uniforms.light0Ambient);

	// L is vector direction from current point (vPos) to the light source (m_uniforms.light0Position)
	CGVec4 L = CGMath::normalize(CGMath::sub(uniforms.light0Position, vPos));
	// calculate dot product of nrm and L
	float NdotL = CGMath::dot(vNrm, L);

	if (NdotL > 0.0F) {
		// diffuse
		diff = CGMath::scale(CGMath::mul(uniforms.materialDiffuse, uniforms.light0Diffuse), NdotL);

		// E is direction from current point (pos) to eye position
		CGVec4 ePos; ePos.set(0.0f, 0.0f, 0.0f, 1.0f);
		CGVec4 E = CGMath::normalize(CGMath::sub(ePos, vPos));
		// H is half vector between L and E
		CGVec4 H = CGMath::normalize(CGMath::add(L, E));

		// specular
		float NdotH = CGMath::dot(vNrm, H);

		if (NdotH > 0.0F)
			spec = CGMath::scale(CGMath::mul(uniforms.materialSpecular, uniforms.light0Specular), pow(NdotH, uniforms.materialShininess));
	}
	// sum up the final output color
	vClr = CGMath::add(CGMath::add(CGMath::add(ambi, diff), spec), emis);
	// clamp color values to range [0,1]
	vClr[A] = uniforms.materialDiffuse[A];
	vClr = CGMath::clamp(vClr, 0, 1);

	// Transform from Eye Space into Clip Space.
	vPos = uniforms.projectionMatrix * vPos;
}
#endif

//---------------------------------------------------------------------------
// Übung 10 - Aufgabe 2   |  Funktion erstellt
//---------------------------------------------------------------------------
#if defined(U10) || defined(U11) || defined(U12)
void perPixelLighingVertexProgram(	const CGVertexAttributes& in,
									CGVertexVaryings& out,
									const CGUniformData& uniforms)
{
	// Get hold of all vertex attributes.
	CGVec4 aPos = in.attributes[CG_POSITION_ATTRIBUTE];
	CGVec4 aNrm = in.attributes[CG_NORMAL_ATTRIBUTE];
	CGVec4 aClr = in.attributes[CG_COLOR_ATTRIBUTE];
	CGVec4 aTex = in.attributes[CG_TEXCOORD_ATTRIBUTE];

	// Get hold of all vertex varyings.
	CGVec4 &vPos = out.varyings[CG_POSITION_VARYING];
	CGVec4 &vNrm = out.varyings[CG_NORMAL_VARYING];
	CGVec4 &vClr = out.varyings[CG_COLOR_VARYING];
	CGVec4 &vTex = out.varyings[CG_TEXCOORD_VARYING];
	CGVec4 &vPEs = out.varyings[CG_POSITION_EYESPACE_VARYING];

	// Default program copies all attributes into all varyings used:
	vPos = aPos; vNrm = aNrm; vClr = aClr; vTex = aTex;

	// Transform from Object Space into Eye Space.
	vPEs = uniforms.modelviewMatrix * aPos;
	vNrm = uniforms.normalMatrix* vNrm;
	CGMath::normalize(vNrm);

	// Transform from Eye Space into Clip Space.
	vPos = uniforms.projectionMatrix * vPEs;
}
#endif

//---------------------------------------------------------------------------
// FRAGMENT PROGRAMME
//---------------------------------------------------------------------------
#if defined(U1) || defined(U2) || defined(U3_1) || defined(U3_2) || defined(U3_3) || defined(U4) || defined(U5) || defined(U6) || defined(U6_4) || defined(U7) || defined(U8) || defined(U9) || defined(U10) || defined(U12) || defined(HA1) || defined(HA3)
//---------------------------------------------------------------------------
// generic "passthorugh" fragment program
void passthroughFragmentProgram(const CGFragmentData& in,
								CGVec4& out,
								const CGUniformData& uniforms)
{
	out = in.varyings[CG_COLOR_VARYING];
}
#endif

//---------------------------------------------------------------------------
// Übung 10 - Aufgabe 2   |  Funktion implementiert
// Übung 11 - Aufgabe 2e  |  Textur-Eigenschaften werden berücksichtigt
//---------------------------------------------------------------------------
#if defined(U10) || defined(U11) || defined(U12)
void perPixelLighingFragmentProgram(const CGFragmentData& in,
	CGVec4& out,
	const CGUniformData& uniforms)
{
	CGVec4 clr = in.varyings[CG_COLOR_VARYING];
	CGVec4 nrm = in.varyings[CG_NORMAL_VARYING];
	CGVec4 txc = in.varyings[CG_TEXCOORD_VARYING];
	CGVec4 pos = in.varyings[CG_POSITION_EYESPACE_VARYING];

	// renormalize the normal
	nrm = CGMath::normalize(nrm);

	// Compute Blinn-Phong reflection model.
	CGVec4 emis, ambi, diff, spec;
	emis.set(0.0f, 0.0f, 0.0f, 0.0f); ambi.set(0.0f, 0.0f, 0.0f, 0.0f);
	diff.set(0.0f, 0.0f, 0.0f, 0.0f); spec.set(0.0f, 0.0f, 0.0f, 0.0f);
	// emission
	emis = uniforms.materialEmission;
	// ambient
	ambi = CGMath::mul(uniforms.materialAmbient, uniforms.light0Ambient);

	// L is vector direction from current point (pos) to the light source (m_uniforms.light0Position)
	CGVec4 L = CGMath::normalize(CGMath::sub(uniforms.light0Position, pos));
	// calculate dot product of nrm and L
	float NdotL = CGMath::dot(nrm, L);

	if (NdotL>0)
	{
		// diffuse
		diff = CGMath::scale(CGMath::mul(uniforms.materialDiffuse, uniforms.light0Diffuse), NdotL);

		// E is direction from current point (pos) to eye position
		CGVec4 ePos; ePos.set(0.0f, 0.0f, 0.0f, 1.0f);
		CGVec4 E = CGMath::normalize(CGMath::sub(ePos, pos));
		// H is half vector between L and E
		CGVec4 H = CGMath::normalize(CGMath::add(L, E));

		// specular
		float NdotH = CGMath::dot(nrm, H);

		if (NdotH > 0.0F)
			spec = CGMath::scale(CGMath::mul(uniforms.materialSpecular, uniforms.light0Specular), pow(NdotH, uniforms.materialShininess));
	}

	// sum up the final output color
	clr = CGMath::add(CGMath::add(CGMath::add(ambi, diff), spec), emis);

	// Berücksichtigung der Textur-eigenschaften
	if (uniforms.sampler.texture != NULL)
		clr = CGMath::mul(clr, uniforms.sampler.texture->sample(txc));

	// Explicitly set alpha of the color
	clr[A] = uniforms.materialDiffuse[A];
	// clamp color values to range [0,1]
	clr = CGMath::clamp(clr, 0, 1);
	out = clr;
}
#endif

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 1.2  |  Funktion erstellt
//---------------------------------------------------------------------------
#if defined(HA3)
void normalVertexProgram(	const CGVertexAttributes& in,
							CGVertexVaryings& out,
							const CGUniformData& uniforms)
{
	out.varyings[CG_POSITION_VARYING] = in.attributes[CG_POSITION_ATTRIBUTE];
	out.varyings[CG_NORMAL_VARYING] = in.attributes[CG_NORMAL_ATTRIBUTE];
	out.varyings[CG_TEXCOORD_VARYING] = in.attributes[CG_TEXCOORD_ATTRIBUTE];

	// Transform from Object Space into Eye Space.
	out.varyings[CG_POSITION_VARYING] = uniforms.modelviewMatrix * out.varyings[CG_POSITION_VARYING];

	// Transform from Eye Space into Clip Space.
	out.varyings[CG_POSITION_VARYING] = uniforms.projectionMatrix * out.varyings[CG_POSITION_VARYING];

	// Set normal as color, transformed into [0,1]-range
	out.varyings[CG_COLOR_VARYING][R] = 0.5f * out.varyings[CG_NORMAL_VARYING][X] + 0.5f;
	out.varyings[CG_COLOR_VARYING][G] = 0.5f * out.varyings[CG_NORMAL_VARYING][Y] + 0.5f;
	out.varyings[CG_COLOR_VARYING][B] = 0.5f * out.varyings[CG_NORMAL_VARYING][Z] + 0.5f;
	out.varyings[CG_COLOR_VARYING][A] = 1.0f;
}
#endif

//---------------------------------------------------------------------------
// Erstellt die View-Matrix
//
// Übung 08 - Aufgabe 1a  |  Funktion erstellt
// Übung 08 - Aufgabe 4a  |  Funktion implementiert
// Übung 09 - Aufgabe 1   |  Refaktorisierung 
//---------------------------------------------------------------------------
#if defined(U8) || defined(U9) || defined(U10) || defined(U11) || defined(HA3)
CGMatrix4x4 cguLookAt(	float eyeX,		float eyeY,		float eyeZ,
	float centerX,	float centerY,	float centerZ,
	float upX,		float upY,		float upZ)
{
	CGMatrix4x4 V;
	CGVec4 f, s, u, up;

	f.set(centerX - eyeX, centerY - eyeY, centerZ - eyeZ, 0.0F);
	f = CGMath::normalize(f);

	up.set(upX, upY, upZ, 0);
	s = CGMath::normalize(CGMath::cross(f, up));

	u = CGMath::cross(s, f);

	f = CGMath::scale(f, -1);

	float R[16] = { s[X],	s[Y],	s[Z],	0,
					u[X],	u[Y],	u[Z],	0,
					f[X],	f[Y],	f[Z],	0,
					0,		0,		0,		1 };
	V.setFloatsFromRowMajor(R);
	V = V * CGMatrix4x4::getTranslationMatrix((-1)*eyeX, (-1)*eyeY, (-1)*eyeZ);
	return V;
}
#endif

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 1.2  |  Funktion erstellt
//---------------------------------------------------------------------------
#if defined(U11) || defined(U12) || defined(HA3)
void renderQuadric(CGQuadric &quadric)
{
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, quadric.getPositionArray());
	ourContext->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, quadric.getNormalArray());
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, quadric.getColorArray());
	ourContext->cgVertexAttribPointer(CG_TEXCOORD_ATTRIBUTE, quadric.getTexCoordArray());
	ourContext->cgDrawArrays(GL_TRIANGLES, 0, quadric.getVertexCount());
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, NULL);
	ourContext->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, NULL);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, NULL);
	ourContext->cgVertexAttribPointer(CG_TEXCOORD_ATTRIBUTE, NULL);
}
#endif

//---------------------------------------------------------------------------
// Hausaufgabe 1  |  Eigenes Bild-Format
//---------------------------------------------------------------------------
#if defined(HA1)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  46		// Framebuffer width.
#define FRAME_HEIGHT 11		// Framebuffer height.
#define FRAME_SCALE  10		// Integer scaling factors (zoom).

#define MATNR 310550		// Matrikelnummer

float pos[7 * 6 * 3 * 2];
float color[7 * 6 * 4 * 2];

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 1c  |  Funktion erstellt
// Hausaufgabe 1 - Aufgabe 1d  |  Frame-Buffer lesen und speichern
//---------------------------------------------------------------------------
#if defined(HA1)
void makeScreenshot()
{
	unsigned char* ssBuffer = (unsigned char*)malloc(4 * FRAME_WIDTH * FRAME_HEIGHT);
	ourContext->cgReadPixels(ssBuffer);

	cgImageWriteCG1("Screenshot.cg1", ssBuffer, FRAME_WIDTH, FRAME_HEIGHT);

	free(ssBuffer);
}
#endif

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 1b  |  programStep erstellt und konfiguriert
// Hausaufgabe 1 - Aufgabe 1c  |  Funktion makeScreenshoot wird bei
//							   |  Tastendruck aufgerufen
//---------------------------------------------------------------------------
void programStep_DrawMatNr()
{
	if (CG1Helper::isKeyReleased('s')) makeScreenshot();

	if (pos[0] == NULL) {
		int matNr = MATNR;

		for (int i = 0; i < 7 * 6 * 3 * 2; i++) {
			pos[i] = 0;
		}
		for (int i = 0; i < 7 * 6 * 4 * 2; i++) {
			color[i] = 1;
		}

		for (int anzeige = 5; anzeige >= 0; anzeige--) {
			int ziffer = matNr % 10;
			matNr = (int)(matNr / 10);

			//		  2
			//		 ---
			//	  4 | 1 | 6
			//		 ---
			//	  3 |   | 5
			//		 ---
			//		  0

			// Segment 0, 1, 2
			for (int i = 0; i < 3; i++) {
				pos[anzeige * 7 * 2 * 3 + i * 3 * 2 + 0] = 1 + anzeige * 8; pos[anzeige * 7 * 2 * 3 + i * 3 * 2 + 1] = i * 5;
				pos[anzeige * 7 * 2 * 3 + i * 3 * 2 + 3] = 4 + anzeige * 8; pos[anzeige * 7 * 2 * 3 + i * 3 * 2 + 4] = i * 5;
			}

			// Segment 3, 4
			for (int i = 0; i < 2; i++) {
				pos[anzeige * 7 * 2 * 3 + (i + 3) * 3 * 2 + 0] = 0 + anzeige * 8; pos[anzeige * 7 * 2 * 3 + (i + 3) * 3 * 2 + 1] = 1 + i * 5;
				pos[anzeige * 7 * 2 * 3 + (i + 3) * 3 * 2 + 3] = 0 + anzeige * 8; pos[anzeige * 7 * 2 * 3 + (i + 3) * 3 * 2 + 4] = 4 + i * 5;
			}

			// Segment 5, 6
			for (int i = 0; i < 2; i++) {
				pos[anzeige * 7 * 2 * 3 + (i + 5) * 3 * 2 + 0] = 5 + anzeige * 8; pos[anzeige * 7 * 2 * 3 + (i + 5) * 3 * 2 + 1] = 1 + i * 5;
				pos[anzeige * 7 * 2 * 3 + (i + 5) * 3 * 2 + 3] = 5 + anzeige * 8; pos[anzeige * 7 * 2 * 3 + (i + 5) * 3 * 2 + 4] = 4 + i * 5;
			}

			// Farbe
			// Segment 0
			if (ziffer != 1 && ziffer != 4 && ziffer != 7) {
				color[anzeige * 7 * 2 * 4 + 0 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 0 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 0 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 0 * 4 * 2 + 6] = 0;
			}

			// Segment 1
			if (ziffer != 0 && ziffer != 1 && ziffer != 7) {
				color[anzeige * 7 * 2 * 4 + 1 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 1 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 1 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 1 * 4 * 2 + 6] = 0;
			}

			// Segment 2
			if (ziffer != 1 && ziffer != 4) {
				color[anzeige * 7 * 2 * 4 + 2 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 2 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 2 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 2 * 4 * 2 + 6] = 0;
			}

			// Segment 3
			if (ziffer == 0 || ziffer == 2 || ziffer == 6 || ziffer == 8) {
				color[anzeige * 7 * 2 * 4 + 3 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 3 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 3 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 3 * 4 * 2 + 6] = 0;
			}

			// Segment 4
			if (ziffer != 1 && ziffer != 2 && ziffer != 3 && ziffer != 7) {
				color[anzeige * 7 * 2 * 4 + 4 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 4 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 4 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 4 * 4 * 2 + 6] = 0;
			}

			// Segment 5
			if (ziffer != 2) {
				color[anzeige * 7 * 2 * 4 + 5 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 5 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 5 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 5 * 4 * 2 + 6] = 0;
			}

			// Segment 6
			if (ziffer != 5 && ziffer != 6) {
				color[anzeige * 7 * 2 * 4 + 6 * 2 * 4 + 1] = 0; color[anzeige * 7 * 4 * 2 + 6 * 4 * 2 + 2] = 0;
				color[anzeige * 7 * 2 * 4 + 6 * 2 * 4 + 5] = 0; color[anzeige * 7 * 4 * 2 + 6 * 4 * 2 + 6] = 0;
			}
		}

		// clear
		ourContext->cgClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		ourContext->cgClear(CG_COLOR_BUFFER_BIT);
		// set capabilities and vertex pointers
		ourContext->cgEnable(CG_USE_BRESENHAM);
		ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, pos);
		ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, color);
		// render
		ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
		ourContext->cgDrawArrays(CG_LINES, 0, 6 * 7 * 2);
	}
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_DrawMatNr);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Hausaufgabe 2  |  Matrixoperationen
//---------------------------------------------------------------------------
#if defined(HA2)

//---------------------------------------------------------------------------
// Hausaufgabe 2 - Aufgabe 2.2  |  CGMatrix testen
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	freopen("Ausagbe.txt", "w", stdout);

	CGMatrix4x4 res, trans, skal, rot;
	CGVec4 vec, resV;

	trans = CGMatrix4x4::getTranslationMatrix(1.0F, 2.0F, 3.0F);
	skal = CGMatrix4x4::getScaleMatrix(2.0F, 2.0F, 1.0F);
	res = trans * skal * trans;
	res.debugPrint("A");

	rot = CGMatrix4x4::getRotationMatrixX(90);
	trans = CGMatrix4x4::getTranslationMatrix(10, 0, 0);
	vec.set(0, 1, 0, 1);
	resV = rot * trans * vec;
	printf("B: %f %f %f %f\n\n", resV[0], resV[1], resV[2], resV[3]);

	rot = CGMatrix4x4::getRotationMatrixY(123);
	rot.transpose();
	rot.debugPrint("C");

	rot = CGMatrix4x4::getRotationMatrixZ(100);
	rot.invert();
	rot.debugPrint("D");

	freopen("CON", "w", stdout);

	printf("Die Ergebnis-Matrizen wurde in Datei \"Ausgabe.txt\" geschrieben.\n");
	system("PAUSE");
}
#endif

//---------------------------------------------------------------------------
// Hausaufgabe 3  |  Quadriken
//---------------------------------------------------------------------------
#if defined(HA3)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  480	// Framebuffer width.
#define FRAME_HEIGHT 320	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

CGQuadric podest, cube, cube2, cylinder, disk, cone, cone2, sphere, sphere2;

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 1.2  |  Funktion erstellt
//---------------------------------------------------------------------------
void drawCoordSys()
{
	static float vertices4axes[6*3] = { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 };
	static float colors4axes[6*4] = { 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
	ourContext->cgEnable(CG_USE_BRESENHAM);
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4axes);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4axes);
	ourContext->cgDrawArrays(CG_LINES, 0, 6);
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, NULL);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, NULL);
}

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 1.2  |  Funktion erstellt
// Hausaufgabe 3 - Aufgabe 3    |  Ändere Vertex-Programm auf Knopf-Druck
//---------------------------------------------------------------------------
void programStep_CGUT()
{
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgEnable(CG_CULL_FACE);
	ourContext->cgEnable(CG_BLEND);
	//ourContext->cgPolygonMode(CG_LINE); // ### Uncomment to check tessellesation ###

	// ### Change to normalVertexProgram to visualize normal vectors ###
	static bool choice = false;
	if (CG1Helper::isKeyReleased('c')) choice = !choice;

	if (choice)
		ourContext->cgUseProgram(modelViewProjectionVertexProgram, passthroughFragmentProgram);
	else
		ourContext->cgUseProgram(normalVertexProgram, passthroughFragmentProgram);

	CGMatrix4x4 projMat;
	projMat = CGMatrix4x4::getFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 50.0f);
	float proj[16]; projMat.getFloatsToColMajor(proj);
	ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX, 1, false, proj);
	static float anim = 0.0f; anim += 0.01f;
	float eyeX = cos(anim)*11.0f, eyeY = 11.0f, eyeZ = sin(anim)*11.0f;
	//eyeX = 0; eyeY = 11; eyeZ = 11;
	CGMatrix4x4 viewT = cguLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);
	CGMatrix4x4 modelviewT;
	float mv[16];

	// SHOW ALL QUADRICS
	for (int ix = -1; ix <= 1; ix++) { // 3 columns
		for (int ih = -1; ih <= 1; ih += 2) { // 2 rows
			// Cylinder as podium.
			modelviewT = viewT * // left-most is the view transformation
				CGMatrix4x4::getTranslationMatrix(ix*5.0f, 0, ih*5.0f) * // place on xz plane
				CGMatrix4x4::getRotationMatrixX(-90); // rotate around X so that Z goes up
			modelviewT.getFloatsToColMajor(mv);
			ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
			renderQuadric(podest);
			modelviewT = modelviewT*
				CGMatrix4x4::getTranslationMatrix(0, 0, 1); // disk as cover (in the rotated COS of the cylinder!)
			modelviewT.getFloatsToColMajor(mv);
			ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
			renderQuadric(disk);

			// The quadric object.
			modelviewT = modelviewT *
				CGMatrix4x4::getTranslationMatrix(0, 0, 1) * // the quadric (in the rotated COS of the cylinder!)
				CGMatrix4x4::getScaleMatrix(0.5, 0.5, 0.5); // but scale down by 50%
			modelviewT.getFloatsToColMajor(mv);
			ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
			if (ih == -1) {
				if (ix == -1) renderQuadric(sphere);
				else if (ix == 0) renderQuadric(cone);
				else if (ix == 1) renderQuadric(cube);
			}
			else {
				if (ix == -1) renderQuadric(sphere2);
				else if (ix == 0) renderQuadric(cube2);
				else if (ix == 1) renderQuadric(cone2);
			}
		}
	}
	// SHOW A SINGLE QUADRIC
	modelviewT = viewT * // reset to view only (model=identity)
		CGMatrix4x4::getScaleMatrix(2, 2, 2);
	modelviewT.getFloatsToColMajor(mv);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
	//renderQuadric(sphere2); // ### Use this for debugging ###
	// and overlay the COS (disable and REenable the depth test)
	ourContext->cgDisable(CG_DEPTH_TEST);
	//drawCoordSys(); // draw the COS defined by the MV-matrix
	ourContext->cgEnable(CG_DEPTH_TEST);
}

//---------------------------------------------------------------------------
// Hausaufgabe 3 - Aufgabe 1.2  |  Funktion erstellt
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	podest.setStandardColor(1, 1, 1, 1);
	podest.createCylinder(15, 1);
	cube.setStandardColor(0.5, 0, 0, 1);
	cube.createBox();
	cube2.setStandardColor(1, 0, 0, 1);
	cube2.createBox(5, 5, 5);
	//cylinder.createCylinder(15,5);
	disk.setStandardColor(1, 1, 1, 1);
	disk.createDisk(15, 2);
	cone.setStandardColor(0, 0.5, 0, 1);
	cone.createCone(20, 5);
	cone2.setStandardColor(0, 1, 0, 1);
	cone2.createCone(0.5f, 20, 5);
	sphere.setStandardColor(0, 0, 0.5, 1);
	sphere.createUVSphere(10, 5);
	sphere2.setStandardColor(0, 0, 1, 1);
	sphere2.createIcoSphere(3);

	CG1Helper::initApplication(ourContext, 640, 640);
	CG1Helper::setProgramStep(programStep_CGUT);
	CG1Helper::runApplication();
	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 01  |  Implementierung Frame-Buffer
//---------------------------------------------------------------------------
#if defined(U1)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  160   // Framebuffer width.
#define FRAME_HEIGHT 100   // Framebuffer height.
#define FRAME_SCALE  5     // Integer scaling factors (zoom).

// Vertex Attribute Arrays.
#define VERTEX_COUNT 2
float vertex[VERTEX_COUNT][3];	// x,y,z
float color[VERTEX_COUNT][4];	  // r,g,b,a

//---------------------------------------------------------------------------
// This is something specifically needed for this application.
typedef struct
{
	float dx;
	float dy;
	float x;
	float y;
} SpringenderPunkt;
SpringenderPunkt a={1.0f, 1.0f, 20.0f, 0.0f};
SpringenderPunkt b={0.5f, -0.75f, 70.0f, 50.0f};

//---------------------------------------------------------------------------
void animateSpringenderPunkt(SpringenderPunkt& p)
{
	if (p.x >=  FRAME_WIDTH-1) p.dx = -fabsf(p.dx);
	if (p.x <=           0.0f) p.dx =  fabsf(p.dx);
	if (p.y >= FRAME_HEIGHT-1) p.dy = -fabsf(p.dy);
	if (p.y <=           0.0f) p.dy =  fabsf(p.dy);
	p.x+=p.dx;
	p.y+=p.dy;
}
//---------------------------------------------------------------------------
// programStep_* functions are always our main application.
void programStep_SpringenderPunkt()
{
	if (CG1Helper::isKeyReleased(CG_KEY_LEFT))  a.dx = -1.0f;
	if (CG1Helper::isKeyReleased(CG_KEY_RIGHT)) a.dx =  1.0f;
	if (CG1Helper::isKeyReleased(CG_KEY_UP))    a.dy =  1.0f;
	if (CG1Helper::isKeyReleased(CG_KEY_DOWN))  a.dy = -1.0f;
	animateSpringenderPunkt(a);
	animateSpringenderPunkt(b);
	
	// render
	///---------------------------------------------------------------------------
	/// Übung 01 - Aufgabe 2a  |  Setzen der Hintergrundfarbe
	///---------------------------------------------------------------------------
	//ourContext->cgClearColor(0.5f, 0.0f, 0.0f, 1.0f);	// Rot
	//ourContext->cgClearColor(1.0f, 1.0f, 0.0f, 1.0f);	// Gelb
	//ourContext->cgClearColor(0.0f, 1.0f, 1.0f, 1.0f);	// Cyan
	//ourContext->cgClearColor(1.0f, 0.0f, 1.0f, 1.0f);	// Magenta
	ourContext->cgClearColor(1.0f, 1.0f, 1.0f, 1.0f);	// Weiß
	//ourContext->cgClearColor(0.8f, 0.8f, 0.8f, 1.0f);	// Grau 20%

	ourContext->cgClear(CG_COLOR_BUFFER_BIT);

	// prepare vertex array for point a
	color[0][0]=1.0f;
	color[0][1]=1.0f;
	color[0][2]=1.0f;
	color[0][3]=1.0f;

	vertex[0][0]=a.x;
	vertex[0][1]=a.y;
	vertex[0][2]=0.0f;
	
	// prepare vertex array for point b
	color[1][0]=0.0f;
	color[1][1]=1.0f;
	color[1][2]=0.0f;
	color[1][3]=1.0f;

	vertex[1][0]=b.x;
	vertex[1][1]=b.y;
	vertex[1][2]=0.0f;
	
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
	
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);

	///---------------------------------------------------------------------------
	/// Übung 01 - Aufgabe 3b  |  Wechsel zwischen Ausgabe als Punkte oder als
	///						   |  Linie
	///---------------------------------------------------------------------------
	//ourContext->cgDrawArrays(CG_POINTS, 0, 2);
	ourContext->cgDrawArrays(CG_LINES, 0, 2);
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_SpringenderPunkt);
	
	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 02  |  Linienrasterisierung nach Bresenham
//---------------------------------------------------------------------------
#if defined(U2)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  160   // Framebuffer width.
#define FRAME_HEIGHT 100   // Framebuffer height.
#define FRAME_SCALE  5     // Integer scaling factors (zoom).

// Vertex Attribute Arrays.
#define VERTEX_COUNT 2
float vertex[VERTEX_COUNT][3];	// x,y,z

//---------------------------------------------------------------------------
// Übung 02 - Aufgabe 1a  |  programStep erstellt
// Übung 02 - Aufgabe 1c  |  Bresenham aktiviert
//---------------------------------------------------------------------------
void programStep_LineBenchmark()
{
	// prepare vertex array for point a
	vertex[0][0] = 0.0f;
	vertex[0][1] = 0.0f;
	vertex[0][2] = 0.0f;

	// prepare vertex array for point b
	vertex[1][0] = FRAME_WIDTH - 1.0f;
	vertex[1][1] = FRAME_HEIGHT - 1.0f;
	vertex[1][2] = 0.0f;

	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgEnable(CG_USE_BRESENHAM);

	for (int i = 0; i < 10000; i++) {
		ourContext->cgDrawArrays(CG_LINES, 0, 2);
	}
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_LineBenchmark);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 1   |  Lineare Interpolation
//---------------------------------------------------------------------------
#if defined(U3_1)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  41		// Framebuffer width.
#define FRAME_HEIGHT 41		// Framebuffer height.
#define FRAME_SCALE  5		// Integer scaling factors (zoom).

float vertexPosition_TestBresenham[32*3] = {
	20, 30, 0, 20, 40, 0, 24, 29, 0, 28, 38, 0, 27, 27, 0, 34, 34, 0, 29, 24, 0, 38, 28, 0,
	30, 20, 0, 40, 20, 0, 29, 16, 0, 38, 12, 0, 27, 13, 0, 34, 6, 0, 24, 11, 0, 28, 2, 0,
	20, 10, 0, 20, 0, 0, 16, 11, 0, 12, 2, 0, 13, 13, 0, 6, 6, 0, 11, 16, 0, 2, 12, 0,
	10, 20, 0, 0, 20, 0, 11, 24, 0, 2, 28, 0, 13, 27, 0, 6, 34, 0, 16, 29, 0, 12, 38, 0
};
float vertexColor_TestBresenham[32*4] = {
	1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1,
	1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1
};

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 1a  |  programStep erstellt
//---------------------------------------------------------------------------
void programStep_TestBresenham()
{
	// clear
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT);
	// set capabilities and vertex pointers
	ourContext->cgEnable(CG_USE_BRESENHAM);
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertexPosition_TestBresenham);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, vertexColor_TestBresenham);
	// render
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_LINES, 0, 32); // 32 vertices for 16 lines.
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_TestBresenham);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 2   |  Dreiecke (Wireframe)
//---------------------------------------------------------------------------
#if defined(U3_2)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  41		// Framebuffer width.
#define FRAME_HEIGHT 41		// Framebuffer height.
#define FRAME_SCALE  5		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 2b  |  programStep erstellt
//---------------------------------------------------------------------------
void programStep_TestTriangle()
{
	// clear
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT);

	// set vertex data
	float vertexPosition_TestTriangle[3*3] = { 0, 0, 0, 41 - 1, 0, 0, 41 / 2, 41 - 1, 0 };
	float vertexColor_TestTriangle[3*4] = { 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1 };
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertexPosition_TestTriangle);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, vertexColor_TestTriangle);

	// render wireframe triangle
	ourContext->cgPolygonMode(CG_LINE);
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3); // 3 vertices for 1 triangle.
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_TestTriangle);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 3   |  Fragment-Clipping
// Übung 04				  |  Dreiecksrasterisierung
//---------------------------------------------------------------------------
#if defined(U3_3) || defined(U4)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  41		// Framebuffer width.
#define FRAME_HEIGHT 41		// Framebuffer height.
#define FRAME_SCALE  5		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Übung 03 - Aufgabe 3b  |  programStep erstellt
// Übung 04 - Aufgabe 1b  |  gefülltes Dreieck erzeugen
//---------------------------------------------------------------------------
void programStep_TestRotatingTriangle()
{
	// clear
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT);

	// Construct vertex data
	static float a=0.0; a+=0.01;
	float ca = cos(a), sa = sin(a);
	float vertexPosition_TestRotTriangle[3*3]={20,20,0, ca*30+20,sa*30+20,0, -sa*20+20,ca*20+20,0};
	float vertexColor_TestRotTriangle[3*4] ={1,0,0,1, 0,0,1,1, 0,1,0,1};

	// and set vertex pointers
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE,	vertexPosition_TestRotTriangle);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE,		vertexColor_TestRotTriangle);

	///---------------------------------------------------------------------------
	/// Übung 04 - Aufgabe 1b  |  gefülltes Dreieck erzeugen
	///---------------------------------------------------------------------------
	// render
	// ourContext->cgPolygonMode(CG_LINE);
	ourContext->cgPolygonMode(CG_FILL);

	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3); // 3 vertices for 1 triangle.
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_TestRotatingTriangle);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 05  |  Backface Culling, Z-Buffer
//---------------------------------------------------------------------------
#if defined(U5)
//---------------------------------------------------------------------------
// Übung 05 - Aufgabe 1a  |  Frame-Buffer-Größe angepasst
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  500	// Framebuffer width.
#define FRAME_HEIGHT 300	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Übung 05 - Aufgabe 1b  |  programStep erstellt
// Übung 05 - Aufgabe 2a  |  Depth - Test aktiviert, nur Linien zeichnen
// Übung 05 - Aufgabe 3e  |  Flächen zeichnen
//---------------------------------------------------------------------------
void programStep_TestBFCandZTest()
{
	// clear
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT/*|CG_DEPTH_BUFFER_BIT*/);

	static float a = 0.0; a += 0.01;
	float ca = cos(a), sa = sin(a);
	float PTS[24] = {	-ca - sa*1.28, -0.6, sa - ca*1.28, ca - sa*1.28, -0.6, -sa - ca*1.28, ca - sa*0.6, 1.28, -sa - ca*0.6,
						-ca - sa*0.6, 1.28, sa - ca*0.6, -ca + sa*0.6, -1.28, sa + ca*0.6, ca + sa*0.6, -1.28, -sa + ca*0.6,
						 ca + sa*1.28, 0.6, -sa + ca*1.28, -ca + sa*1.28, 0.6, sa + ca*1.28 };
#define R3 1,0,0,1, 1,0,0,1, 1,0,0,1
#define G3 0,1,0,1, 0,1,0,1, 0,1,0,1
#define B3 0,0,1,1, 0,0,1,1, 0,0,1,1
#define C3 0,1,1,1, 0,1,1,1, 0,1,1,1
#define M3 1,0,1,1, 1,0,1,1, 1,0,1,1
#define Y3 1,1,0,1, 1,1,0,1, 1,1,0,1
#define P(i) PTS[3*i+0]*60+100,PTS[3*i+1]*50+200,PTS[3*i+2]*0.1

	// construct vertex data
	float vertices4triangles[54 * 3] = {
		//clipped triangles
		0, 0, -1.1, 50, 0, -1.1, 0, 50, -1.1,
		50, 0, +1.1, 50, 50, +1.1, 0, 50, +1.1,

		//visible triangles
		//green tri
		300, 0, -0.5, 480, 0, -0.5, 300, 100, -0.5,
		//red tri
		300, 0, -0.5, 480, 100, -0.5, 480, 0, -0.5,
		//blue tri
		250, 180, 0.5, 500, 240, -0.5, 250, 240, 0.5,
		//yellow tri
		300, 280, 0, 350, 150, 0, 400, 260, 0,

		//rotating cube
		P(0), P(1), P(2), P(0), P(2), P(3), P(1), P(5), P(6), P(1), P(6), P(2), P(3), P(2), P(6), P(3), P(6), P(7),
		P(0), P(5), P(1), P(0), P(4), P(5), P(0), P(3), P(7), P(0), P(7), P(4), P(4), P(7), P(6), P(4), P(6), P(5) };
	float colors4triangles[54 * 4] = { B3, B3, G3, R3, C3, Y3, G3, G3, R3, R3, B3, B3, Y3, Y3, C3, C3, M3, M3
	};
#undef P
#undef R3
#undef B3
#undef B3
#undef C3
#undef M3
#undef Y3

	///---------------------------------------------------------------------------
	/// Übung 05 - Aufgabe 2a  |  Depth-Test aktiviert, nur Linien zeichnen
	/// Übung 05 - Aufgabe 3e  |  Flächen zeichnen
	///---------------------------------------------------------------------------
	ourContext->cgEnable(CG_DEPTH_TEST);
	// ourContext->cgPolygonMode(CG_LINE);

	//set Attrib Pointers
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4triangles);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4triangles);

	//set Shaders and draw
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 54);
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_TestBFCandZTest);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 06  |  Transparenz
//---------------------------------------------------------------------------
#if defined(U6)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  500	// Framebuffer width.
#define FRAME_HEIGHT 300	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Übung 06 - Aufgabe 1a  |  programStep erstellt
// Übung 06 - Aufgabe 3b  |  Sortierung der Dreiecke nach Z-Wert
//---------------------------------------------------------------------------
void programStep_TestBlendingZSort()
{
	static bool anim_blend = false, anim_circle = true;
	if (CG1Helper::isKeyReleased('b')) anim_blend = !anim_blend;
	if (CG1Helper::isKeyReleased('c')) anim_circle = !anim_circle;

	const int N = 20; // We define a set of N triangles for a resolution of 500x300
	static float b = asin(0.4), c = 0.0; b += (anim_blend) ? 0.01 : 0.0; c += (anim_circle) ? 0.01 : 0.0;
	float vertices[N*3*3], colors[N*3*4]; // again, don’t do this locally as we do!

	for (int i = 0; i<N; i++) {
		float rgba[4] = { (i + 1) & 0x01 ? 1 : 0, (i + 1) & 0x02 ? 1 : 0, (i + 2) & 0x04 ? 1 : 0, sin(b)*0.5 + 0.5 };
		float pos[3] = { i*10, i*10, -i*0.01 };
		if (i >= 10) { // all triangles >= 10 will be rotating ones.
			float a2 = c + float(i - 10) / float(N - 10)*6.283;
			pos[X] = 300 + sin(a2)*100.0; pos[Y] = 100 - cos(a2)*50.0; pos[Z] = cos(a2)*0.9;
		}
		for (int j = 0; j<3; j++) memcpy(colors + 12*i + 4*j, rgba, sizeof(float)*4);
		for (int j = 0; j<3; j++) memcpy(vertices + 9*i + 3*j, pos, sizeof(float)*3);
		vertices[9*i + 6 + X] += 25.0;
		vertices[9*i + 3 + X] += 50.0;
		vertices[9*i + 6 + Y] += 75.0;
	}

	// The index array describes the order of the triangles.
	// [0,2,1] would mean to render the first, third and then second triangle.
	int indexArray[N];
	for (int i = 0; i<N; i++) indexArray[i] = i; // initialize with identity [0,1,2...]

	bool ready;
	do {
		ready = true;
		for (int i = 1; i < N; ++i) {
			if (vertices[3 * 3 * indexArray[i] + Z] > vertices[3 * 3 * (indexArray[i - 1]) + Z]) {
				int temp = indexArray[i];
				indexArray[i] = indexArray[i - 1];
				indexArray[i - 1] = temp;
				ready = false;
			}
		}
	} while (!ready);

	ourContext->cgClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgEnable(CG_CULL_FACE);
	ourContext->cgEnable(CG_BLEND);
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors);
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	// We use the index array by passing the correct vertex offset for each triangle.
	for (int i = 0; i<N; i++)
		ourContext->cgDrawArrays(CG_TRIANGLES, 3
		*
		indexArray[i], 3);
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_TestBlendingZSort);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 06 - Aufgabe 4  |  Awesome Triangle
//---------------------------------------------------------------------------
#if defined(U6_4)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  500	// Framebuffer width.
#define FRAME_HEIGHT 300	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Übung 06 - Aufgabe 4   |  programStep erstellt
//---------------------------------------------------------------------------
void programStep_AwesomeTriangle()
{
	const int N = 20;
	static float a = 0.0; a += 0.01;
	if (a >= 0.5) a = 0.0;
	float vertices[3*3*N], colors[3*4*N]; // again, don’t do this locally as we do!
	vertices[3*0 + X] = 0; vertices[3*0 + Y] = 0; vertices[3*0 + Z] = 0;
	colors[4*0 + R] = 1.0f; colors[4*0 + G] = 0; colors[4*0 + B] = 0; colors[4*0 + A] = 0.2f;
	vertices[3*1 + X] = 500; vertices[3*1 + Y] = 0; vertices[3*1 + Z] = 0;
	colors[4*1 + R] = 0.0f; colors[4*1 + G] = 1.0f; colors[4*1 + B] = 0; colors[4*1 + A] = 0.2f;
	vertices[3*2 + X] = 250; vertices[3*2 + Y] = 433; vertices[3*2 + Z] = 0;
	colors[4*2 + R] = 0.0f; colors[4*2 + G] = 0; colors[4*2 + B] = 1.0f; colors[4*2 + A] = 0.2f;
	
	float tempVertices[3*3];
	float tempColors[3*4];

	for (int i = 0; i < 19; i++) {
		memcpy(tempColors, &colors[3*4*i], 3*4*sizeof(float));
		
		tempVertices[3*0 + X] = a*vertices[3*3*i + 3*1 + X] + (1 - a)*vertices[3*3*i + 3*0 + X];
		tempVertices[3*0 + Y] = a*vertices[3*3*i + 3*1 + Y] + (1 - a)*vertices[3*3*i + 3*0 + Y];
		tempVertices[3*0 + Z] = a*vertices[3*3*i + 3*1 + Z] + (1 - a)*vertices[3*3*i + 3*0 + Z];
		
		tempVertices[3*1 + X] = a*vertices[3*3*i + 3*2 + X] + (1 - a)*vertices[3*3*i + 3*1 + X];
		tempVertices[3*1 + Y] = a*vertices[3*3*i + 3*2 + Y] + (1 - a)*vertices[3*3*i + 3*1 + Y];
		tempVertices[3*1 + Z] = a*vertices[3*3*i + 3*2 + Z] + (1 - a)*vertices[3*3*i + 3*1 + Z];
		
		tempVertices[3*2 + X] = a*vertices[3*3*i + 3*0 + X] + (1 - a)*vertices[3*3*i + 3*2 + X];
		tempVertices[3*2 + Y] = a*vertices[3*3*i + 3*0 + Y] + (1 - a)*vertices[3*3*i + 3*2 + Y];
		tempVertices[3*2 + Z] = a*vertices[3*3*i + 3*0 + Z] + (1 - a)*vertices[3*3*i + 3*2 + Z];
		
		memcpy(&vertices[3*3*(i + 1)], tempVertices, 3*3*sizeof(float));
		memcpy(&colors[3*4*(i + 1)], tempColors, 3*4*sizeof(float));
	}
	ourContext->cgClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	//ourContext->cgEnable(CG_DEPTH_TEST);
	//ourContext->cgEnable(CG_CULL_FACE);
	ourContext->cgEnable(CG_BLEND);
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors);
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 20
		*
		3);
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_AwesomeTriangle);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 07  |  Perspektivische Projektion
//---------------------------------------------------------------------------
#if defined(U7)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  500	// Framebuffer width.
#define FRAME_HEIGHT 300	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Zeichnet einen bunten Quader in der hinteren Ecke auf grauem Grund
//
// Übung 07 - Aufgabe 1a  |  programStep erstellt
// Übung 07 - Aufgabe 2c  |  Neues Vertex-Programm wird verwendet
// Übung 07 - Aufgabe 3c  |  ViewPort hinzugefügt
//---------------------------------------------------------------------------
void programStep_ProjectionTest()
{
	// Create a unit-cube which is later (manually) transformed.
	// Hint: this is just a hack as long as we have no model transformation,
	// i. e. do not attempt this at home!
	float vertices4cube[12*3*3] = { 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,	// FRONT
									0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,	// BACK
									0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0,	// LEFT
									1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1,	// RIGHT
									0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0,	// TOP
									0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0 };	// BOTTOM
	float colors4cube[12*3*4];
	for (int i = 0; i<36; i++) {
		// compute colors from temporary vertex positions:
		colors4cube[4*i + 0] = vertices4cube[3*i + 0];
		colors4cube[4*i + 1] = vertices4cube[3*i + 1];
		colors4cube[4*i + 2] = vertices4cube[3*i + 2];
		colors4cube[4*i + 3] = 1.0f;
		// compute (manually!) transformed vertex positions:
		vertices4cube[3*i + 0] = vertices4cube[3*i + 0]*0.5f + 0.5f;
		vertices4cube[3*i + 1] = vertices4cube[3*i + 1]*0.5f - 1.0f;
		vertices4cube[3*i + 2] = vertices4cube[3*i + 2]*0.5f - 2.0f;
	}

	// ground plane
	CGMatrix4x4 projMat;
	float f = 1.1;
	projMat = CGMatrix4x4::getFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 2.0f);
	float proj[16]; projMat.getFloatsToColMajor(proj);
	ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX, 1, false, proj);

	ourContext->cgClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgEnable(CG_CULL_FACE); // !
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4cube);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4cube);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 12*3);

	// Plus: draw a ground plane:
	float vertices4ground[6*3] = { -1, -1, -1, 1, -1, -1, 1, -1, -3, -1, -1, -1, 1, -1, -3, -1, -1, -3 };
	float colors4ground[6*4];
	for (int i = 6; i--;) {
		float* c = colors4ground + 4*i; 
		c[0] = c[1] = c[2] = 0.8f;
		c[3] = 1.0f;
	}
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4ground);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4ground);
	ourContext->cgUseProgram(projectionVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 2*3);

	ourContext->cgViewport(100, 100, 300, 200);
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_ProjectionTest);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 08  |  Transformation
//---------------------------------------------------------------------------
#if defined(U8)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH 480	// Framebuffer width.
#define FRAME_HEIGHT 320	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

float vertices4trunk[3*3]	= {-0.2,0,0.1,		0.2,0,0.1,		0,2,0};
float colors4trunk[3*4]		= {0.5,0.25,0,1,	0.5,0.25,0,1,	0.5,0.25,0,1};
float vertices4leafs[3*3]	= {0,0,0,			2,0,0.2,		0,2,0.2};
float colors4leafs[3*4]		= {1,1,1,1,			0,0.8,0,1,		0,0.5,0,1};
float vertices4ground[6*3]	= {-10,0,-10, 10,0,10, 10,0,-10, -10,0,-10, -10,0,10, 10,0,10};
float colors4ground[6*4]	= {0,1,0,1, 1,1,1,1, 0,1,0,1, 0,1,0,1, 1,1,1,1, 1,1,1,1};

//---------------------------------------------------------------------------
// Übung 08 - Aufgabe 1a  |  Funktion erstellt
//---------------------------------------------------------------------------
void drawGround(CGMatrix4x4 viewT)
{
	// GROUND
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4ground);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4ground);
	float floatValues[16]; viewT.getFloatsToColMajor(floatValues);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 6);
}

//---------------------------------------------------------------------------
// Übung 08 - Aufgabe 1a  |  Funktion erstellt
// Übung 08 - Aufgabe 3a  |  Stamm und Blätter geeignet transformiert
//---------------------------------------------------------------------------
void drawTree(CGMatrix4x4 transform)
{
	CGMatrix4x4 Ttrunk, Tleaf1, Tleaf2, Tleaf3;
	float floatValues[16];
	// ZA5 a) // mehrere Layer davon zeichnen

	Ttrunk = transform * CGMatrix4x4::getIdentityMatrix();
	Tleaf1 = transform * CGMatrix4x4::getTranslationMatrix(0, 2.5, 0) * CGMatrix4x4::getRotationMatrixZ(225);
	Tleaf2 = transform * CGMatrix4x4::getTranslationMatrix(0, 3.0, 0) * CGMatrix4x4::getScaleMatrix(0.8, 0.8, 0.8) * CGMatrix4x4::getRotationMatrixZ(225);
	Tleaf3 = transform * CGMatrix4x4::getTranslationMatrix(0, 3.5, 0) * CGMatrix4x4::getScaleMatrix(0.5, 0.5, 0.5) * CGMatrix4x4::getRotationMatrixZ(225);

	// TRUNK
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4trunk);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4trunk);
	Ttrunk.getFloatsToColMajor(floatValues);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3);

	// LEAVES
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4leafs);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4leafs);

	for(int i=3; i--;) {
		if(i==0) Tleaf1.getFloatsToColMajor(floatValues);
		else if(i==1) Tleaf2.getFloatsToColMajor(floatValues);
		else Tleaf3.getFloatsToColMajor(floatValues);
		ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
		ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3);
	}
}

//---------------------------------------------------------------------------
// Übung 08 - Aufgabe 1a  |  Funktion erstellt
//---------------------------------------------------------------------------
CGMatrix4x4 cguPerspective(float fov_y, float aspect, float zNear, float zFar)
{
	// ZA6 a)
	CGMatrix4x4 P;
	return P;
}

//---------------------------------------------------------------------------
// Übung 08 - Aufgabe 1a  |  Funktion erstellt
// Übung 08 - Aufgabe 2b  |  Verwendetes Vertex-Programm geändert
//---------------------------------------------------------------------------
void programStep_HappyHolidays()
{
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgEnable(CG_CULL_FACE);
	CGMatrix4x4 projMat = CGMatrix4x4::getFrustum(-0.062132f, 0.062132f, -0.041421f, 0.041421f, 0.1f, 50.0f);
	float proj[16]; projMat.getFloatsToColMajor(proj);
	ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX, 1, false, proj);
	ourContext->cgUseProgram(modelViewProjectionVertexProgram, passthroughFragmentProgram);
	// A4 b)
	// Camera rotating around center on r=15 circle.
	static float anim = 0.0; anim+=0.01;
	float eyeX = cos(anim)*15.0f, eyeY = 15.0f, eyeZ = sin(anim)*15.0f;
	CGMatrix4x4 viewT = cguLookAt(eyeX,eyeY,eyeZ, 0,2,0, 0,1,0);
	//CGMatrix4x4 viewT = CGMatrix4x4::getTranslationMatrix(0.0f, -5.0, -25.0f);

	drawGround(viewT);
	for (int i = 10; i--;) {
		float prX = float(i % 7) / 6.0f*16.0f - 8.0f, // [0,6]->[-8,+8]
			prZ = float(i % 4) / 3.0f*16.0f - 8.0f; // [0,3]->[-8,+8]
		CGMatrix4x4 treeT = viewT * CGMatrix4x4::getTranslationMatrix(prX, 0, prZ);
		drawTree(treeT);
	}
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0));           //init random seed

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

	CG1Helper::setProgramStep(programStep_HappyHolidays);

	CG1Helper::runApplication();

	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 09  |  Beleuchtung I
// Übung 10  |  Beleuchtung II
//---------------------------------------------------------------------------
#if defined(U9) || defined(U10)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  480	// Framebuffer width.
#define FRAME_HEIGHT 320	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Light and material properties, which will later be passed as uniforms.
float	rgbaWhite10[4] = { 1, 1, 1, 1 },
		rgbaWhite01[4] = { 0.1, 0.1, 0.1, 1.0 },
		rgbaWhite05[4] = { 0.5, 0.5, 0.5, 1.0 },
		rgbaGreen[4] = { 0, 1, 0, 1.0 },
		rgbaRed[4] = { 1, 0, 0, 1 },
		rgbaBlack[4] = { 0, 0, 0, 1 };
float shininess = 32.0f;

//---------------------------------------------------------------------------
// Übung 09 - Aufgabe 1   |  programStep erstellt
// Übung 10 - Aufgabe 3   |  Vergleich der beiden Beleuchtungsmethoden
//---------------------------------------------------------------------------
void programStep_Lighting()
{
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgEnable(CG_CULL_FACE);

#if defined(U10)
	static bool choice = true;

	if (CG1Helper::isKeyReleased('c')) choice = !choice;

	if (!choice)
		ourContext->cgUseProgram(perPixelLighingVertexProgram, perPixelLighingFragmentProgram);
	else
#endif
		ourContext->cgUseProgram(perVertexLighingVertexProgram, passthroughFragmentProgram);

	// set LIGHT properties (uniforms) with
	// - low (white) ambient intensity (global diffuse lighting)
	// - medium (white) diffuse and specular intensity
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_AMBIENT, 1, rgbaWhite01);
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_DIFFUSE, 1, rgbaWhite05);
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_SPECULAR, 1, rgbaWhite05);
	// set MATERIAL properties (uniforms) with// - (red only) ambient and diffuse reflectance
	// - (all channels) specular reflectance plus shininess
	// - disable emission
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_AMBIENT, 1, rgbaRed);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE, 1, rgbaRed);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR, 1, rgbaWhite10);// TRY: rgbaGreen
	ourContext->cgUniform1fv(CG_ULOC_MATERIAL_SHININESS, 1, &shininess);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION, 1, rgbaBlack); // TRY: rgbaWhite01
	// set projection matrix
	float proj[16];
	CGMatrix4x4::getFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 50.0f).getFloatsToColMajor(proj);
	ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX, 1, false, proj);
	// set modelview matrix and normal matrix
	float mv[16], nm[16];
	CGMatrix4x4 viewT = cguLookAt(0, 2, 2.5, 0, 0, 0, 0, 1, 0);
	CGMatrix4x4 modelT = CGMatrix4x4::getRotationMatrixY(40);
	CGMatrix4x4 modelviewT = viewT*modelT;
	modelviewT.getFloatsToColMajor(mv);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
	modelviewT.getFloatsToColMajor(nm); // Get normal matrix (column major!).
	nm[12] = nm[13] = nm[14] = 0.0f; // Reduce to 3x3 matrix (column major!).
	nm[3] = nm[7] = nm[11] = 0.0f;
	nm[15] = 1.0f;
	CGMatrix4x4 normalMatrix; normalMatrix.setFloatsFromColMajor(nm);
	normalMatrix.invert(); normalMatrix.transpose();
	normalMatrix.getFloatsToColMajor(nm); // Get the correct values of (MV.3x3)^-1^T
	ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX, 1, false, nm);
	renderTestSphere(ourContext, modelviewT, 1.0f);
	// Specify light in Eye Space here
	static float anim = 0.0f; anim += 0.01f;
	CGVec4 lightPos; lightPos.set(cos(anim)*1.4, 1.4, sin(anim)*1.4, 1);
	CGVec4 lightPosES = modelviewT*lightPos;
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_POSITION, 1, lightPosES.elements);
	/// Visualize the light source smaller sphere
	modelviewT = modelviewT*CGMatrix4x4::getTranslationMatrix(lightPos[0], lightPos[1], lightPos[2]);
	modelviewT.getFloatsToColMajor(mv);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_AMBIENT, 1, rgbaBlack);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE, 1, rgbaBlack);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR, 1, rgbaBlack);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION, 1, rgbaWhite10);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
	renderTestSphere(ourContext, modelviewT, 0.05f);
}

//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0)); //init random seed
	createTestSphere(3); //create sphere vertices
	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);
	CG1Helper::setProgramStep(programStep_Lighting);
	CG1Helper::runApplication();
	return 0;
}
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
// Übung 11  |  Texturierung
//---------------------------------------------------------------------------
#if defined(U11)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  480	// Framebuffer width.
#define FRAME_HEIGHT 320	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Light and material properties, which will later be passed as uniforms.
float	rgbaWhite10[4] = { 1, 1, 1, 1 },
		rgbaWhite01[4] = { 0.1, 0.1, 0.1, 1.0 },
		rgbaWhite05[4] = { 0.5, 0.5, 0.5, 1.0 },
		rgbaGreen[4] = { 0, 1, 0, 1.0 },
		rgbaRed[4] = { 1, 0, 0, 1 },
		rgbaBlack[4] = { 0, 0, 0, 1 };
float shininess = 32.0f;

CGQuadric sphere;
int myTexture1;

//---------------------------------------------------------------------------
// Übung 11 - Aufgabe 1a  |  Funktion erstellt
// Übung 11 - Aufgabe 2d  |  Ändere Wrap-Modi auf Tastendruck
// Übung 11 - Aufgabe 4b  |  Ändere Filter-Modi auf Tastendruck
//---------------------------------------------------------------------------
void programStep_Texturing()
{
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgDisable(CG_CULL_FACE);
	ourContext->cgUseProgram(perPixelLighingVertexProgram, perPixelLighingFragmentProgram);

	// set LIGHT properties (uniforms) with
	// - low (white) ambient intensity (global diffuse lighting)
	// - medium (white) diffuse and specular intensity
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_AMBIENT, 1, rgbaWhite01);
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_DIFFUSE, 1, rgbaWhite05);
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_SPECULAR, 1, rgbaWhite05);

	// Set any Light specified in Eye Space here: float lightPos[4] = {...};
	// ourContext->cgUniform4fv(CG_ULOC_LIGHT0_POSITION,1,lightPos);
	// set MATERIAL properties (uniforms) with
	// - (green only) ambient and diffuse reflectance
	// - (all channels) specular reflectance plus shininess
	// - disable emission
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_AMBIENT, 1, rgbaWhite10);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE, 1, rgbaWhite10);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR, 1, rgbaWhite10);
	ourContext->cgUniform1fv(CG_ULOC_MATERIAL_SHININESS, 1, &shininess);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION, 1, rgbaBlack);

	// set projection matrix
	float proj[16];
	CGMatrix4x4::getFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 50.0f).getFloatsToColMajor(proj);
	ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX, 1, false, proj);

	// set modelview matrix and normal matrix
	float mv[16], nm[16];
	CGMatrix4x4 viewT = cguLookAt(0, 2, 2, 0, 0, 0, 0, 1, 0);
	CGMatrix4x4 modelT = CGMatrix4x4::getRotationMatrixY(40);
	CGMatrix4x4 modelviewT = viewT * modelT;
	modelviewT.getFloatsToColMajor(mv);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
	modelviewT.getFloatsToColMajor(nm); // Get normal matrix (column major!).
	nm[12] = nm[13] = nm[14] = 0.0f; // Reduce to 3x3 matrix (column major!).
	nm[3] = nm[7] = nm[11] = 0.0f;
	nm[15] = 1.0f;
	CGMatrix4x4 normalMatrix; normalMatrix.setFloatsFromColMajor(nm);
	normalMatrix.invert(); normalMatrix.transpose();
	normalMatrix.getFloatsToColMajor(nm); // Get the correct values of (MV.3x3)^-1^T
	ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX, 1, false, nm);

	// Specify light in Object Space here: (because we want it relative to the object)
	static float anim = 0.0f; anim += 0.01f;
	CGVec4 lightPos; lightPos.set(cosf(anim)*1.4f, 1.4f, sinf(anim)*1.4f, 1);
	CGVec4 lightPosES = modelviewT * lightPos;
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_POSITION, 1, lightPosES.elements);
	float vertices4quad[6*3] = { -1, 0, -1, -1, 0, 1, 1, 0, -1, 1, 0, -1, -1, 0, 1, 1, 0, 1 };
	float normals4quad[6*3] = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	float colors4quad[6*4] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	float texcoords4quad1[6*2] = { 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1 };
	float texcoords4quad2[6*2] = { 0, 0, 0, 2, 2, 0, 2, 0, 0, 2, 2, 2 };
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4quad);
	ourContext->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, normals4quad);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4quad);

	// 4b) use different texcoord array
	ourContext->cgVertexAttribPointer(CG_TEXCOORD_ATTRIBUTE, texcoords4quad2);

	// activate texture unit 0 with texture myTexture1: (sorry for the name)
	// 2d)/4b) test different wrap modes/filters
	static bool cWrap, cFilter;
	if (CG1Helper::isKeyReleased('w')) cWrap = !cWrap;
	if (CG1Helper::isKeyReleased('f')) cFilter = !cFilter;

	ourContext->cgActiveTexture_cgBindTexture_cgTexParameter(	0, myTexture1,
																(cFilter ? CGTexture2D::CG_NEAREST : CGTexture2D::CG_LINEAR),
																(cWrap ? CGTexture2D::CG_CLAMP : CGTexture2D::CG_REPEAT));
	ourContext->cgUniform1i(CG_ULOC_SAMPLER, 0);
	ourContext->cgDrawArrays(GL_TRIANGLES, 0, 2*3);

	// deactivate texture unit 0 by binding negative texture name
	ourContext->cgActiveTexture_cgBindTexture_cgTexParameter(	0, -1,
																CGTexture2D::CG_NEAREST,
																CGTexture2D::CG_CLAMP);
	
	/// Visualize the light source smaller sphere
	modelviewT = modelviewT*CGMatrix4x4::getTranslationMatrix(lightPos[0], lightPos[1], lightPos[2]);
	modelviewT = modelviewT*CGMatrix4x4::getScaleMatrix(0.05f, 0.05f, 0.05f);
	modelviewT.getFloatsToColMajor(mv);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION, 1, rgbaWhite05);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
	renderQuadric(sphere);
}

//---------------------------------------------------------------------------
// Übung 11 - Aufgabe 1a  |  Funktion erstellt
// Übung 11 - Aufgabe 1c  |  Pfad der Textur geändert
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	sphere.setStandardColor(1, 1, 1, 1);
	sphere.createUVSphere(10, 10);
	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);
	CG1Helper::setProgramStep(programStep_Texturing);
	// create textures just once:
	int imgWidth, imgHeight;
	unsigned char* imgData4ub = cgImageLoad("Texture\\checker.tga", &imgWidth, &imgHeight);
	myTexture1 = ourContext->cgGenTexture_cgBindTexture_cgTexImage2D(imgWidth, imgHeight, imgData4ub);
	CG1Helper::runApplication();
	return 0;
}
#endif

//---------------------------------------------------------------------------
// Übung 12  |  Anwendung
//---------------------------------------------------------------------------
#if defined(U12)
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  400	// Framebuffer width.
#define FRAME_HEIGHT 300	// Framebuffer height.
#define FRAME_SCALE  2		// Integer scaling factors (zoom).

//------------------------------------------------------------------------
// Übung 12 - Aufgabe 2a  |  Variable 'testdata' erstellt
// Übung 12 - Aufgabe 3a  |  Variable 'cube' erstellt
// Übung 12 - Aufgabe 4c  |  Variablen für Interaktion erstellt
//---------------------------------------------------------------------------
TestDataSet testdata;
CGQuadric cube;
int		global_CameraMode		= 0; // We will hardcode three positions.
float	global_CameraAnimation	= 0; // Camera animation parameter.
int		global_LightMode		= 0; // We will hardcode three light positions.
float	global_LightAnimation	= 0; // Light animation parameter.
int		global_LightingMode		= 0; // LightING mode (per-Pixel/per-Vertex/none).

//------------------------------------------------------------------------
// Pass matrix as modelview to pipeline and update normal matrix.
//
// Übung 12 - Aufgabe 1c  |  Funktion erstellt
//---------------------------------------------------------------------------
void setModelViewMatrixUniform(CGMatrix4x4 modelview) {
	float mv[16]; modelview.getFloatsToColMajor(mv);
	ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX, 1, false, mv);
	// Build and set normalmatrix:
	float nm[16]; modelview.getFloatsToColMajor(nm);
	nm[12] = nm[13] = nm[14] = nm[3] = nm[7] = nm[11] = 0.0f; nm[15] = 1.0f;
	CGMatrix4x4 normalMatrix; normalMatrix.setFloatsFromColMajor(nm);
	normalMatrix.invert(); normalMatrix.transpose();
	normalMatrix.getFloatsToColMajor(nm);
	ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX, 1, false, nm);
}

//------------------------------------------------------------------------
// Übung 12 - Aufgabe 2a  |  Funktion erstellt
//---------------------------------------------------------------------------
void programStep_DataVisualization()
{
	///------------------------------------------------------------------------
	/// Übung 12 - Aufgabe 4a  |  Lade Bild auf Tastendruck
	/// Übung 12 - Aufgabe 4b  |  Veränderung der einzelnen Quader auf Tastendruck
	/// Übung 12 - Aufgabe 4d  |  Änderung der Kamera-, Licht und Beleuchtungsmodi
	///						   |  auf Tastendruck
	///---------------------------------------------------------------------------
#pragma region Input handlers.
	if (CG1Helper::isKeyReleased('1')) testdata.loadFromImage("Texture\\smiley.tga");
	if (CG1Helper::isKeyReleased('2')) testdata.loadFromImage("Texture\\text.tga");
	if (CG1Helper::isKeyReleased('3')) testdata.loadFromImage("Texture\\hypnotoad.tga");

	if (CG1Helper::isKeyPressed(32)) testdata.process();

	if (CG1Helper::isKeyReleased('c')) global_CameraMode = (global_CameraMode + 1) % 3;
	if (CG1Helper::isKeyReleased('l')) global_LightMode = (global_LightMode + 1) % 3;
	if (CG1Helper::isKeyReleased('p')) global_LightingMode = (global_LightingMode + 1) % 3;
#pragma endregion

#pragma region Per frame processed events (animations!) or other calculations.
	// some_global_or_static_variable += 0.5;

	// Extract center of dataset for each frame.
	float dataCenter[3] = { testdata.maxX() / 2, testdata.maxY() / 2, testdata.maxZ() / 2 };
#pragma endregion

	///------------------------------------------------------------------------
	/// Übung 12 - Aufgabe 4e  |  Änderung der Beleuchtungsmodi auf Tastendruck
	///------------------------------------------------------------------------
#pragma region CG (GL) Setup.
	ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT | CG_DEPTH_BUFFER_BIT);

	// Make sure you understand what all those do, else ASK US!
	ourContext->cgEnable(CG_DEPTH_TEST); // We need depth testing.
	ourContext->cgEnable(CG_CULL_FACE); // We don’t need back faces.
	// We want lights and per fragment lighting.
	switch (global_LightingMode) {
		case 0: ourContext->cgUseProgram(perPixelLighingVertexProgram, perPixelLighingFragmentProgram); break;
		case 1: ourContext->cgUseProgram(perVertexLighingVertexProgram, passthroughFragmentProgram); break;
		case 2: ourContext->cgUseProgram(modelViewProjectionVertexProgram, passthroughFragmentProgram); break;
	}
#pragma endregion

#pragma region Light and material properties.
	// Some useful float arrays.
	float	rgbaWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f }, // pure white, alpha 1
			rgbaGray[4] = { 0.5f, 0.5f, 0.5f, 1.0f },
			rgbaDarkGray[4] = { 0.1f, 0.1f, 0.1f, 1.0f },
			rgbaBlack[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 16.0f;
	// Set LIGHT properties.
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_AMBIENT, 1, rgbaDarkGray);
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_DIFFUSE, 1, rgbaWhite);
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_SPECULAR, 1, rgbaGray);

	// Set MATERIAL properties (until someone changes them).
	// You can also build a setMaterialUniforms()-Function.
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_AMBIENT, 1, rgbaWhite);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE, 1, rgbaWhite);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR, 1, rgbaWhite);
	ourContext->cgUniform1fv(CG_ULOC_MATERIAL_SHININESS, 1, &shininess);
	ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION, 1, rgbaBlack);
#pragma endregion

#pragma region Projection setup.
	float proj[16];
	// Set a (perspective) frustum with the [-1,1]^2 rect on the nearplane at 1.0.
	float nearDist = 1.0f, farDist = 100.0f; // Is this a good value for farDist?
	// What would happen if we used 1000000.0f or 2.0f? Make sure you understand this, else ASK US!
	CGMatrix4x4::getFrustum(-1.0f, 1.0f, -1.0f, 1.0f, nearDist, farDist).getFloatsToColMajor(proj);
	ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX, 1, false, proj);
#pragma endregion

#pragma region View (camera) setup.
	CGMatrix4x4 viewT; // Set camera by creating the ’view’-part of the modelview-matrix.
	// Moving the camera is the same as moving the scene inverse.
	// So center the scene and push it away from the camera (using two transformations).
	// You can also use cgLookAt() here (but don’t right now).
	viewT = CGMatrix4x4::getTranslationMatrix(-dataCenter[0], -dataCenter[1], -dataCenter[2]);

	// Aufgabe 4 (g): Rotate the camera position according to the global_CameraMode and global_CameraAnimation

	viewT = CGMatrix4x4::getTranslationMatrix(0, 0, -20) * viewT;
#pragma endregion

	///------------------------------------------------------------------------
	/// Übung 12 - Aufgabe 4f  |  Änderung der Lichtmodi auf Tastendruck
	///------------------------------------------------------------------------
#pragma region Light position setup.
	CGVec4 lightPos; // Light position in object space (using same coordinate frame as data).
	
	switch (global_LightMode) {
		case 0: lightPos.set(testdata.maxX() / 2, testdata.maxY() / 2, testdata.maxZ() * 2, 1); break;
		default: 
			global_LightAnimation += 1.0F;
			lightPos.set(testdata.maxX() / 2, testdata.maxY() / 2, testdata.maxZ() * 2, 1);
			lightPos = CGMatrix4x4::getRotationMatrixY(global_LightAnimation) * lightPos;
			break;
		case 2: lightPos.set(testdata.maxX() / 2, testdata.maxY() / 2, testdata.maxZ() * 2, 1); break;
	}

	// BUT the pipeline needs the eye space light position!
	// Moving from scene object space to eye space using ModelView matrix
	// (scene has no model transformation, so model-part is identity).
	// Make sure you understand this, else ASK US!
	CGVec4 lightPosES = viewT*lightPos;
	ourContext->cgUniform4fv(CG_ULOC_LIGHT0_POSITION, 1, lightPosES.elements);
#pragma endregion

	///------------------------------------------------------------------------
	/// Übung 12 - Aufgabe 3b  |  Schleife für alle TestDataItems erstellt
	/// Übung 12 - Aufgabe 3c  |  Model-View-Matrix erstellt
	/// Übung 12 - Aufgabe 3d  |  Diffuser Farbanteil wird gesetzt
	/// Übung 12 - Aufgabe 3e  |  Zeichne 'cube'
	///------------------------------------------------------------------------
#pragma region Happy rendering.
	for (int i = 0; i < testdata.size(); i++) {
		TestDataSet::TestDataItem item = testdata.get(i);

		CGMatrix4x4 modelT =	CGMatrix4x4::getTranslationMatrix(item.x, item.y, item.z) *
								CGMatrix4x4::getScaleMatrix(0.5F, 0.5F, 0.5F);
		setModelViewMatrixUniform(viewT * modelT);

		float rgba[4] = { item.r, item.g, item.b, 1.0F };
		ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE, 1, rgba);

		renderQuadric(cube);
	}
#pragma endregion
}

//---------------------------------------------------------------------------
// Übung 12 - Aufgabe 1b  |  Funktion erstellt
// Übung 12 - Aufgabe 3a  |  Erzeuge nicht unterteilte Box
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand(time(0)); //init random seed

	cube.createBox();

	CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);
	CG1Helper::setProgramStep(programStep_DataVisualization);
	CG1Helper::runApplication();
	return 0;
}
#endif