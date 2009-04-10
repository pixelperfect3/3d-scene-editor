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

using namespace std;

#define KEY_ESC 27

Program p = {
	true, false, -3, 640, 480, 60, 1, 1500, 
	"Wiimote IR visualizer :: Ethan Blackwelder"
};

bool flip = true; //flip by default
WiiMoteClient *wiimote;

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

void DrawScene() {
	glPushMatrix();

	glPointSize(5);
	glColor3f(1, 1, 1);
	
	glBegin(GL_POINTS);
	glVertex3d(0, 0, 0);
	glEnd();

	if (flip) {
		glScaled(-1.0, 1.0, 1.0);
	}
	glScaled(2.0 * p.screen_width / (VRPN_WIIMOTE_MAX_IR_X + 0.0), 2.0 * p.screen_height / (VRPN_WIIMOTE_MAX_IR_Y + 0.0), 1.0);
	glTranslated(-p.screen_width / 2.0, -p.screen_height / 2.0, 0.0);

	//TODO : put a "last updated" variable into wiimote_client.
	for (int ii = 0; ii < 4; ii++) {
		double x = wiimote->ir_x[ii];
		double y = wiimote->ir_y[ii];
		int size = wiimote->ir_size[ii];
		if (size > 0) {
			glPointSize(size);
			glBegin(GL_POINTS);
			glVertex3d(x, y, 0);
			glEnd();
			std::cout << "IR[" << ii << "]: { " << x << ", " << y << " } (" << size << " pts).\n";
		} else {
			//std::cout << "IR[" << ii << "]: NA.\n";
		}
	}
	glPopMatrix();
}

void UpdateTrackers() {
	if (wiimote) {
		wiimote->updateFromServer();
		vrpn_SleepMsecs(2);
	}
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

	gluLookAt(0,0,-1000, 0,0,0, 0,1,0); //for wheelchair position rendering.
	DrawScene();

	glFlush();
	checkGLErrors("An OpenGL error occurred while drawing");
}

void ParseArgs(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Usage: wiimote_client.exe <tracker_name1>\n";
		exit(-1);
	} else if (argc == 2) {
		wiimote = new WiiMoteClient(argv[1], 1);
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
	if (wiimote != NULL) {
		wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_IR, true);
	}
}

int main(int argc, char** argv) {
	ParseArgs(argc, argv);
    init();
    glutMainLoop();
	shutdown();
	std::cout << "Bye!\n";
    return 0;
}
