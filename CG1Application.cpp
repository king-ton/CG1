// Welche Übung soll ausgeführt werden?
#define UEBUNG5

// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>           // for time() in srand()

// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"

//---------------------------------------------------------------------------
// GLOBALE VARIABLEN
//---------------------------------------------------------------------------
CGContext *ourContext;

//---------------------------------------------------------------------------
// VERTEX PROGRAMME
//---------------------------------------------------------------------------
#if defined(UEBUNG1) || defined(UEBUNG2) || defined(UEBUNG3_1) || defined(UEBUNG3_2) || defined(UEBUNG3_3) || defined(UEBUNG4) || defined(UEBUNG5)
//---------------------------------------------------------------------------
// generic "passthorugh" vertex program
void passthroughVertexProgram(const CGVertexAttributes& in,
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
// FRAGMENT PROGRAMME
//---------------------------------------------------------------------------
#if defined(UEBUNG1) || defined(UEBUNG2) || defined(UEBUNG3_1) || defined(UEBUNG3_2) || defined(UEBUNG3_3) || defined(UEBUNG4) || defined(UEBUNG5)
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
// Übung 01  |  Implementierung Frame-Buffer
//---------------------------------------------------------------------------
#if defined(UEBUNG1)
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
#if defined(UEBUNG2)
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
#if defined(UEBUNG3_1)
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
#if defined(UEBUNG3_2)
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
#if defined(UEBUNG3_3) || defined(UEBUNG4)
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
#if defined(UEBUNG5)
//---------------------------------------------------------------------------
// Übung 05 - Aufgabe 1a  |  Frame-Buffer-Größe angepasst
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  500	// Framebuffer width.
#define FRAME_HEIGHT 300	// Framebuffer height.
#define FRAME_SCALE  5		// Integer scaling factors (zoom).

//---------------------------------------------------------------------------
// Übung 05 - Aufgabe 1b  |  programStep erstellt
// Übung 05 - Aufgabe 2a  |  Depth - Test aktiviert, nur Linien zeichnen
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
	///---------------------------------------------------------------------------
	ourContext->cgEnable(CG_DEPTH_TEST);
	ourContext->cgPolygonMode(CG_LINE);

	//set Attrib Pointers
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4triangles);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4triangles);

	//set Shaders and draw
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	ourContext->cgDrawArrays(CG_TRIANGLES, 0, 54);
}
//---------------------------------------------------------------------------

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

