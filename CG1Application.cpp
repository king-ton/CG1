// Welche Übung soll ausgeführt werden?
#define UEBUNG1

// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>           // for time() in srand()

// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"

//---------------------------------------------------------------------------
// Übung 1  |  Implementierung Color-Buffer
//---------------------------------------------------------------------------
#ifdef UEBUNG1
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  160   // Framebuffer width.
#define FRAME_HEIGHT 100   // Framebuffer height.
#define FRAME_SCALE  5     // Integer scaling factors (zoom).
CGContext *ourContext;

//---------------------------------------------------------------------------
// generic "passthorugh" vertex program
void passthroughVertexProgram(const CGVertexAttributes& in,
							  CGVertexVaryings& out,
							  const CGUniformData& uniforms)
{
	out.varyings[CG_POSITION_VARYING] = in.attributes[CG_POSITION_ATTRIBUTE];
	out.varyings[CG_NORMAL_VARYING]   = in.attributes[CG_NORMAL_ATTRIBUTE];
	out.varyings[CG_COLOR_VARYING]    = in.attributes[CG_COLOR_ATTRIBUTE];
	out.varyings[CG_TEXCOORD_VARYING] = in.attributes[CG_TEXCOORD_ATTRIBUTE];
}
//---------------------------------------------------------------------------
// generic "passthorugh" fragment program
void passthroughFragmentProgram(const CGFragmentData& in,
								CGVec4& out,
								const CGUniformData& uniforms)
{
	out = in.varyings[CG_COLOR_VARYING];
}

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

// Vertex Attribute Arrays.
#define VERTEX_COUNT 2
float vertex[VERTEX_COUNT][3];	// x,y,z
float color[VERTEX_COUNT][4];	  // r,g,b,a

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
	ourContext->cgClearColor(0.5f, 0.0f, 0.0f, 1.0f);	// Rot
	//ourContext->cgClearColor(1.0f, 1.0f, 0.0f, 1.0f);	// Gelb
	//ourContext->cgClearColor(0.0f, 1.0f, 1.0f, 1.0f);	// Cyan
	//ourContext->cgClearColor(1.0f, 0.0f, 1.0f, 1.0f);	// Magenta
	//ourContext->cgClearColor(1.0f, 1.0f, 1.0f, 1.0f);	// Weiß
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
