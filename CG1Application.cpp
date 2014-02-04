// Welche Übung soll ausgeführt werden?
#define U8

// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>           // for time() in srand()

//---------------------------------------------------------------------------
// Hausaufgabe 1 - Aufgabe 2c  |  CGImageFile hinzugefügt
//---------------------------------------------------------------------------
// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"
#include "CGImageFile.h"

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
#if defined(U8)
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
// FRAGMENT PROGRAMME
//---------------------------------------------------------------------------
#if defined(U1) || defined(U2) || defined(U3_1) || defined(U3_2) || defined(U3_3) || defined(U4) || defined(U5) || defined(U6) || defined(U6_4) || defined(U7) || defined(U8) || defined(HA1)
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
#define FRAME_WIDTH  580	// Framebuffer width.
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
CGMatrix4x4 cguLookAt(	float eyeX,		float eyeY,		float eyeZ,
						float centerX,	float centerY,	float centerZ,
						float upX,		float upY,		float upZ)
{
	// A4 a)
	CGMatrix4x4 V;
	return V;
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
	//static float anim = 0.0; anim+=0.01;
	//float eyeX = cos(anim)*15.0f, eyeY = 15.0f, eyeZ = sin(anim)*15.0f;
	//CGMatrix4x4 viewT = cguLookAt(eyeX,eyeY,eyeZ, 0,2,0, 0,1,0);
	CGMatrix4x4 viewT = CGMatrix4x4::getTranslationMatrix(0.0f, -5.0, -25.0f);

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