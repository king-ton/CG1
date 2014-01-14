//This class is responsible for setting up and maintaining the actual application
//It should not be neccessary to change anything in here or to look at it at all.


#ifndef CG1HELPER_H
#define CG1HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "CG.h"
#include "CGContext.h"

#ifndef M_PI    // for Windows
#define M_PI 3.14159265358979323846
#endif


class CGContext;  //Forward declarations
class CGFrameBuffer;

class CG1Helper
{
public:
	CG1Helper(){};
	~CG1Helper(){};
  
	static void AttachSoftwareFrameBuffer(CGFrameBuffer* _buffer)
	{
		smFrameBuffer = _buffer;
	}
	static void initApplication(CGContext* &_context, int width, int height, int windowScale=1);
	static void runApplication();
	static void setProgramStep(void (*progLoop)());
	static bool isKeyPressed(unsigned char key) { return pressedKeys[key];};
	static bool isKeyReleased(unsigned char key){ bool t = releasedKeys[key]; releasedKeys[key] = false; return t;};

private:
	static void m_displayFunc();
	static void m_idleFunc();
	static void m_reshapeFunc(int w, int h);
	static void m_keyboardFunc (unsigned char key, int x, int y);
	static void m_keyboardUpFunc (unsigned char key, int x, int y);
	static void m_specialKeyboardFunc (int key, int x, int y);
	static void m_specialKeyboardUpFunc (int key, int x, int y);
	static CGFrameBuffer* smFrameBuffer;
	static CGContext* smContext;
	static void (*smProgramStep)();
	static GLuint FBTexture;
	static int glcontextWidth,glcontextHeight;
	static bool pressedKeys[256];
	static bool releasedKeys[256];
};



#endif
