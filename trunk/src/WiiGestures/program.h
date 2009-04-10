#pragma once

struct Program {
	bool ignore_tracking;
	bool stereo;
	int ipd;
	int screen_width;
	int screen_height;
	GLdouble fovy;
	GLdouble zNear;
	GLdouble zFar;
	char *title;
};
