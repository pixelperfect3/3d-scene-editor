#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <windows.h>
#include <quat.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define WIIMOTE_RIGHT_HAND_COORDINATES

#include "program.h"
#include "wiimote_client.h"
#include "wiimote_gesturer.h"

using namespace std;

#define KEY_ESC 27

Program p = {
	true, false, -3, 640, 480, 60, 1, 1500, 
	"Wiimote IR visualizer :: Ethan Blackwelder"
};

bool flip = true; //flip by default
WiiMoteClient *wiimote;
WiiMoteGesturer *gesturer;
double point[2] = { 0, 0 };
DWORD last_time = 0;
double degrees = 90;

void checkGLErrors(char *reason) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << reason << ": ";
		switch (error) {
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM\n";
			break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_VALUE\n";
			break;                    
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION\n";
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "GL_STACK_OVERFLOW\n";
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "GL_STACK_UNDERFLOW\n";
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY\n";
			break;
		default:
			std::cout << "some other error!\n";
			break;
		}
		exit(-1);
	}
}

void shutdown() {
	if (wiimote) {
		wiimote->disconnect();
	}
}

void SpecialKeyPressed(int key, int x, int y) {}

void KeyPressed(unsigned char key, int x, int y) {
    switch (key) {
		case KEY_ESC:
			shutdown();
			std::cout << "Bye Bye!\n\n";
			exit(0);
			break;
		case 'f':
			flip = !flip;
			std::cout << "Horizontal window flipping set to " << (flip ? "on" : "off") << ".\n";
			break;
		case 'i':
			std::cout << "Setting IR mode on.\n";
			wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_MOTION_SENSE, false);
			wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_IR, true);
			break;
		case 'o':
			std::cout << "Setting IR mode off.\n";
			wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_IR, false);
			wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_MOTION_SENSE, true);
			break;
	}
}


void UpdateTrackers() {
	if (wiimote) {
		wiimote->updateFromServer();
		vrpn_SleepMsecs(10);
	}
}

void ToggleMouseCursor(bool useMouse) {
	//TODO : stub
}

void MouseMoved(double mouse[3], double old_mouse[3], double distance[2]) {
	//std::cout << "Mouse: { " << mouse[0] << ", " << mouse[1] << " }\n";
}
void Rotate(double delta) {
	degrees += delta;
}
void Translate(double distance[2]) {
	//std::cout << "Distance: { " << distance[0] << ", " << distance[1] << " }\n";
	point[0] += distance[0];
	point[1] += distance[1];
}
void DrawPoint2D(const double x, const double y, const GLfloat size, const GLfloat color[3]) {
	glPointSize(size);
	glBegin(GL_POINTS);
		glColor3fv(color);
		glVertex3d(x, y, 0);
	glEnd();
}
void DrawPoint2D(const double point[2], const GLfloat size, const GLfloat color[3]) {
	DrawPoint2D(point[0], point[1], size, color);
}

void DrawPoints(int num_points, double points[4][3]) { //called by WiiMoteGesturer::frameStarted
	glPushMatrix();
	gluLookAt(0,0,-1000, 0,0,0, 0,1,0);

	glPushMatrix();
	if (flip) {
		glScaled(-1.0, 1.0, 1.0);
	}
	glScaled(p.screen_width / 2.0, p.screen_height, 1.0);
	
	glPushMatrix();
	glTranslated(-point[0], -point[1], 0);
	glRotated(degrees, 0, 0, 1.0);
	GLfloat color[3] = { 1, 1, 1 };
	DrawPoint2D( 0,  0, 5, color);
	DrawPoint2D(0.1,  0, 3, color);
	DrawPoint2D( 0, 0.1, 3, color);
	glPopMatrix();

	const GLfloat colors[4][3] = {
		{ 0, 0, 1 },
		{ 0, 1, 0 },
		{ 1, 0, 0 },
		{ 0, 1, 1 },
	};
	//TODO : put a "last updated" variable into wiimote_client.
	for (int ii = 0; ii < num_points; ii++) {
		double x = points[ii][0];
		double y = points[ii][1];
		GLfloat size = points[ii][2];
		if (size > 0) {
			DrawPoint2D(x, y, size, colors[ii]);
			//std::cout << "IR[" << ii << "]: { " << x << ", " << y << " } (" << size << " px).\n";
		}
	}
	if (num_points == 2) {
		double tmp[2];
		Midpoint(points[0], points[1], tmp);
		DrawPoint2D(tmp, 2, colors[3]);
	}
	glPopMatrix();
	glPopMatrix(); //gluLookAt
}
void RenderScene() {
	UpdateTrackers();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(p.fovy, p.screen_width/p.screen_height, p.zNear, p.zFar);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
   	glViewport(0, 0, p.screen_width, p.screen_height);

	DWORD tickCount = GetTickCount();
	gesturer->frameStarted(tickCount - last_time);
	last_time = tickCount;

	//gluLookAt(0,0,-1000, 0,0,0, 0,1,0);
	//DrawScene();

	glFlush();
	checkGLErrors("An OpenGL error occurred while drawing");
}

void ParseArgs(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Usage: wiimote_client.exe <tracker_name1>\n";
		exit(-1);
	} else if (argc == 2) {
		wiimote = new WiiMoteClient(argv[1], 1);
		gesturer = new WiiMoteGesturer(wiimote);
		wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_IR, true);
		std::cout << "Using WiiMote \"" << argv[1] << "\"\n";
	}
}

void init() {
	int argc = 1;
	char *ignore = "ignore";
    glutInit(&argc, &ignore);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(p.screen_width, p.screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(p.title);

	glutIdleFunc(RenderScene);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(KeyPressed);
	glutSpecialFunc(SpecialKeyPressed);

	glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	last_time = GetTickCount();
}

int main(int argc, char** argv) {
	ParseArgs(argc, argv);
    init();
    glutMainLoop();
	shutdown();
	std::cout << "Bye!\n";
    return 0;
}
