#include "CGContext.h"
#include "CGMatrix.h"

static float *sphereV = NULL, *sphereC=NULL;
static int spheretris=0;
static float *circleV = NULL, *circleC = NULL;
static int circlelines = 0;

//---------------------------------------------------------------------------
// Pass the current context and modelview matrix for correct placement!
void drawTestSphere2D(CGContext *context, CGMatrix4x4 mv, float radius)
{
	float values[16]; (mv*CGMatrix4x4::getScaleMatrix(radius,radius,1)).getFloatsToColMajor(values);
	context->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, circleV);
	context->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE,    circleC);
	context->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,values);
	context->cgDrawArrays(CG_LINES, 0, circlelines*2);
	mv.getFloatsToColMajor(values); // reset mv (assuming it was set before)
	context->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,values);
	context->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, NULL);
	context->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, NULL);
}
//---------------------------------------------------------------------------
// Pass the current context and modelview matrix for correct placement!
static void renderTestSphere(CGContext *context, CGMatrix4x4 mv, float radius)
{
	float values[16]; (mv*CGMatrix4x4::getScaleMatrix(radius,radius,radius)).getFloatsToColMajor(values);
	context->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,values);
	context->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, sphereV);
	context->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, sphereV);
	context->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, sphereC);
	context->cgDrawArrays(CG_TRIANGLES, 0, spheretris*3);
	mv.getFloatsToColMajor(values); // reset mv (assuming it was set before)
	context->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,values);
	context->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, NULL);
	context->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, NULL);
}
//---------------------------------------------------------------------------
// Please don't try to understand this code:
//---------------------------------------------------------------------------
static void createTestSphere(int subdiv)
{
	int st = 8, st_= 8; for(int s=subdiv; s--; st_*=4); int f3 = sizeof(float)*3, f4 = sizeof(float)*4;
	float* v=sphereV = (float*)realloc(sphereV,f3*3*st_), *c=sphereC = (float*)realloc(sphereC,f4*3*st_);
	float p_[18]={-1,0,+1,-1,0,-1,+1,0,-1,+1,0,+1,0,+1,0,0,-1,0};
	int i_[24]={4,0,3,4,3,2,4,2,1,4,1,0,5,3,0,5,2,3,5,1,2,5,0,1};
	for(int i=24; i--; ) memcpy(v+3*i,p_+3*i_[i],f3); for(int s=subdiv; s--; ) { 
	st_=st*4; for(int i=st; i--; ) { int o=i*4*9; memcpy(p_,v+i*9,f3*3);
	float *v1=p_, *v2=p_+3, *v3=p_+6, *y=v+o; for(int d=3; d--; ) {
	y[d]=v1[d];y[12+d]=v2[d];y[24+d]=v3[d]; y[3+d]=y[9+d]=y[30+d]=(v1[d]+v2[d])/2;
	y[15+d]=y[21+d]=y[33+d]=(v2[d]+v3[d])/2; y[6+d]=y[18+d]=y[27+d]=(v3[d]+v1[d])/2; } } 
	st=st_; for(int i=st*3;i--;) { float *y=v+3*i, 
	li=1.0f/sqrt(y[0]*y[0]+y[1]*y[1]+y[2]*y[2]); y[0]*=li; y[1]*=li; y[2]*=li; } }
	for(int i=st*3;i--;c+=4) {c[0]=c[2]=0;c[1]=c[3]=0.5;} spheretris=st;
}
//---------------------------------------------------------------------------
// This code can be understood:
//---------------------------------------------------------------------------
static void createTestSphere2D(int NCircVert)
{
	circlelines = NCircVert;
	circleV = (float*)realloc(circleV, 2*3*NCircVert*sizeof(float)); //double vertices
	circleC = (float*)realloc(circleC, 2*4*NCircVert*sizeof(float)); //double vertices
  
	// Draw a circle which perpendicular to the z-axis in Eye Space
	// Note that each vertex appears twice
	static const float deg2rad = M_PI/180.0f;
	const float dt = 360.0f/NCircVert;
	float x, y;
	for (int i=1; i<NCircVert; ++i) {
		x = cosf(i*dt*deg2rad), y = sinf(i*dt*deg2rad);
		int j=2*i-1; //index in array
		circleV[3*j+0] = x;
		circleV[3*j+1] = y;
		circleV[3*j+2] = 0;
		circleV[3*j+3] = x;
		circleV[3*j+4] = y;
		circleV[3*j+5] = 0;
	}

	for (int i=0; i<2*NCircVert; ++i) {
		circleC[4*i+0] = 1;
		circleC[4*i+1] = 1;
		circleC[4*i+2] = 1;
		circleC[4*i+3] = 0.5;
	}

	// Set the first and last vertices to close the loop
	circleV[0] = 1;
	circleV[1] = 0;
	circleV[2] = 0;
	circleV[6*NCircVert-3] = 1;
	circleV[6*NCircVert-2] = 0;
	circleV[6*NCircVert-1] = 0;
}
//--------------------------------------------------------------------------
