//This class is responsible for setting up and maintaining the actual application
//It should not be neccessary to change anything in here or to look at it at all.


#include "CG1Helper.h"

double appstarttime = 0;

#ifdef WIN32 // Windows Code fuer Zeitmessung und VSync

#include "windows.h"

LARGE_INTEGER clocks;

double GetTime(void)
{ 
	LARGE_INTEGER gtime;
	if (QueryPerformanceCounter(&gtime))
		return ((double)gtime.QuadPart)/(double)clocks.QuadPart;
	return 0.0;
}

double init_timer(void)
{
	if (!QueryPerformanceFrequency(&clocks)) {
		/* error */
		clocks.QuadPart=0;
		printf ("Fehler: Timer konnte nicht initialisiert werden\n");
	} else printf ("Timer initialisiert\n");

	appstarttime=GetTime();
	return appstarttime;
}

typedef BOOL (__stdcall *wglSwapIntervalFunc)(int interval);

void DisableVSync(void)
{
	wglSwapIntervalFunc wglSwapInterval = (wglSwapIntervalFunc) wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapInterval)
		wglSwapInterval(0);
}

#else // Linux Version fuer Zeitmessung und VSync

#include <time.h>

double GetTime(void)
{
	struct timespec ts;
	ts.tv_sec=0; ts.tv_nsec=0;
	clock_gettime (CLOCK_REALTIME,&ts);
	double just=((double)ts.tv_sec)+((double)ts.tv_nsec)/1000000000.0;
	return just;
}

double init_timer(void)
{
	printf ("Timer initialisiert\n");
	appstarttime=GetTime();
	return appstarttime;
}



typedef void (*_glSwapIntervalFunc)(int interval);
_glSwapIntervalFunc   _glSwapInterval = NULL;

void DisableVSync(void) {}

#endif
double GetMilliSec(void)
{ 
	return 1000.0*(GetTime()-appstarttime);
}
double GetMicroSec(void)
{ 
	return 1000000.0*(GetTime()-appstarttime);
}

void CG1Helper::initApplication(CGContext* &_context, int width, int height, int windowScale)
{
	init_timer();
	_context = new CGContext(width,height);
	smContext = _context;
	//smContext.cgViewport(100,100);
	int argc = 1;
	char** argv = new char*[1];
	argv[0] = new char[2];
	argv[0][0] = 'a';
	argv[0][1] = 0;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_RGBA |  GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA | GLUT_MULTISAMPLE);
	int scale = windowScale>0?windowScale:1;
	glutInitWindowSize(width*scale, height*scale);
	glutCreateWindow("CG1 Template");

	glGenTextures(1, &FBTexture);
	glBindTexture(GL_TEXTURE_2D,FBTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D,0);

	glutDisplayFunc(m_displayFunc);
	glutReshapeFunc(m_reshapeFunc);
	glutIdleFunc(m_idleFunc);
	glutKeyboardFunc(m_keyboardFunc);
	glutKeyboardUpFunc(m_keyboardUpFunc);
	glutSpecialFunc(m_specialKeyboardFunc);
	glutSpecialUpFunc(m_specialKeyboardUpFunc);
	delete [] argv[0];
	delete [] argv;
}
void CG1Helper::runApplication()
{
	glutMainLoop();
}
void CG1Helper::setProgramStep(void (*progLoop)())
{
	smProgramStep = progLoop;
}
void drawGrid(int width, int height, int scale)
{
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	float inc = 2.0f*scale/(float(width)-1.0f/scale);
	for (float x = -1; x <= 1; x+=inc)
	{
		glVertex2f(x,-1);
		glVertex2f(x, 1);
	}
	inc = 2.0f*scale/(float(height)-1.0f/scale);
	for (float y = -1; y <= 1; y+=inc)
	{
		glVertex2f(-1,y);
		glVertex2f( 1,y);
	}
	glEnd();
}
double current_time;
void CG1Helper::m_displayFunc()
{
	char WinTitle [100];
  
	double Framerate = 0;
	double MSPF = 0;
  
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double m = GetMicroSec();
 
	smProgramStep();

	Framerate = 1000000.0/(GetMicroSec()-current_time);
	MSPF = (GetMicroSec()-m)*0.001;
	current_time=GetMicroSec();
#ifdef WIN32
	sprintf_s(WinTitle, sizeof(WinTitle),"Computergraphik I Template /// %3.1f FPS (%3.4f ms per actual draw)", Framerate, MSPF);
#else
	snprintf(WinTitle, sizeof(WinTitle),"Computergraphik I Template /// %3.1f FPS (%3.4f ms per actual draw)", Framerate, MSPF);
#endif
	glutSetWindowTitle(WinTitle);

	if(smFrameBuffer->colorBuffer.getDataPointer()) {
		glBindTexture(GL_TEXTURE_2D,FBTexture);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,smFrameBuffer->getWidth(),smFrameBuffer->getHeight(),GL_RGBA,GL_UNSIGNED_BYTE,smFrameBuffer->colorBuffer.getDataPointer());
    
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(-1,-1);
			glTexCoord2f(1,0);
			glVertex2f( 1,-1);
			glTexCoord2f(1,1);
			glVertex2f( 1, 1);
			glTexCoord2f(0,1);
			glVertex2f(-1, 1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);
		int scale = glcontextWidth/smFrameBuffer->getWidth();
		if (scale>=5) drawGrid(glcontextWidth, glcontextHeight, scale);
	}
	glutSwapBuffers();
}
void CG1Helper::m_idleFunc()
{
	glutPostRedisplay();
}
void CG1Helper::m_reshapeFunc(int w, int h)
{
	// We allow only full multiples of the framebuffer size smaller than the given size:
	int scale = w/smFrameBuffer->getWidth();
	if(scale<=0) scale = 1;
	glcontextWidth  = scale*smFrameBuffer->getWidth();
	glcontextHeight = scale*smFrameBuffer->getHeight();
	glutReshapeWindow(glcontextWidth, glcontextHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	//gluPerspective (45, (double)w/(double)h, 1, 20000);
	glOrtho(-1,1,-1,1,-1,1);
	glViewport (0, 0, w, h);
	glMatrixMode (GL_MODELVIEW); // Sicherheitshalber 
	glLoadIdentity ();           // Modellierungsmatrix einstellen};
}
void CG1Helper::m_keyboardFunc (unsigned char key, int x, int y)
{
	pressedKeys[key] = true;
	switch (key) {
		// Escape
		case 27:
			exit (0);
			break;
	}
}
void CG1Helper::m_keyboardUpFunc (unsigned char key, int x, int y)
{
	// Problem: if 'S' was pressed and shift is released first, 's' flags as released.
	// So if 's' is released, test if 'S' is pressed and release that one instead.
	// We do this only for A-Z and also assume that this is not a problem for a->A
	if(key>=97 && key<=122 && pressedKeys[key-32]) {
		pressedKeys[key-32]  = false;
		pressedKeys[key]     = false;
		releasedKeys[key-32] = true;
		return;
	}
	pressedKeys[key] = false;
	releasedKeys[key] = true;
}
void CG1Helper::m_specialKeyboardFunc (int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			pressedKeys[CG_KEY_UP] = true;
			break;
		case GLUT_KEY_DOWN:
			pressedKeys[CG_KEY_DOWN] = true;
			break;
		case GLUT_KEY_LEFT:
			pressedKeys[CG_KEY_LEFT] = true;
			break;
		case GLUT_KEY_RIGHT:
			pressedKeys[CG_KEY_RIGHT] = true;
			break;
		case GLUT_KEY_PAGE_UP:
			pressedKeys[CG_KEY_PAGE_UP] = true;
			break;
		case GLUT_KEY_PAGE_DOWN:
			pressedKeys[CG_KEY_PAGE_DOWN] = true;
			break;
		case GLUT_KEY_HOME:
			pressedKeys[CG_KEY_HOME] = true;
			break;
		case GLUT_KEY_END:
			pressedKeys[CG_KEY_END] = true;
			break;
		case GLUT_KEY_INSERT:
			pressedKeys[CG_KEY_INSERT] = true;
			break;
		default:
			pressedKeys[key]     = true;
			break;
	}
}
void CG1Helper::m_specialKeyboardUpFunc (int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			releasedKeys[CG_KEY_UP] = true;
			pressedKeys[CG_KEY_UP] = false;  
			break;
		case GLUT_KEY_DOWN:
			releasedKeys[CG_KEY_DOWN] = true;
			pressedKeys[CG_KEY_DOWN] = false;
			break;
		case GLUT_KEY_LEFT:
			releasedKeys[CG_KEY_LEFT] = true;
			pressedKeys[CG_KEY_LEFT] = false;
			break;
		case GLUT_KEY_RIGHT:
			releasedKeys[CG_KEY_RIGHT] = true;
			pressedKeys[CG_KEY_RIGHT] = false;
			break;
		case GLUT_KEY_PAGE_UP:
			releasedKeys[CG_KEY_PAGE_UP] = true;
			pressedKeys[CG_KEY_PAGE_UP] = false;
			break;
		case GLUT_KEY_PAGE_DOWN:
			releasedKeys[CG_KEY_PAGE_DOWN] = true;
			pressedKeys[CG_KEY_PAGE_DOWN] = false;
			break;
		case GLUT_KEY_HOME:
			releasedKeys[CG_KEY_HOME] = true;
			pressedKeys[CG_KEY_HOME] = false;
			break;
		case GLUT_KEY_END:
			releasedKeys[CG_KEY_END] = true;
			pressedKeys[CG_KEY_END] = false;
			break;
		case GLUT_KEY_INSERT:
			releasedKeys[CG_KEY_INSERT] = true;
			pressedKeys[CG_KEY_INSERT] = false;
			break;
		default:
			releasedKeys[key]     = true;
			pressedKeys[key]     = false;
			break;
	}
}

CGFrameBuffer* CG1Helper::smFrameBuffer = 0;
CGContext* CG1Helper::smContext = 0;
void (*CG1Helper::smProgramStep)() = 0;
GLuint CG1Helper::FBTexture = 0;
int CG1Helper::glcontextWidth = 0;
int CG1Helper::glcontextHeight = 0;
bool CG1Helper::pressedKeys[256] = {0,};
bool CG1Helper::releasedKeys[256] = {0,};
