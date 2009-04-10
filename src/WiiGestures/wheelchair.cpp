#include <iostream>

#include "wheelchair.h"

// TODO : minimize the MIN_DELTA_TIME.
#define MIN_DELTA_TIME 50

/* Methods for class Wheelchair */

Wheelchair::Wheelchair(void) {
	resetPositionAndOrientation();
}

Wheelchair::~Wheelchair(void) {}

void Wheelchair::resetPositionAndOrientation() {
	q_vec_set(my_pos_and_quat.xyz, 0, 0, 0);
	my_pos_and_quat.quat[Q_X] = 0;
	my_pos_and_quat.quat[Q_Y] = 0;
	my_pos_and_quat.quat[Q_Z] = 0;
	my_pos_and_quat.quat[Q_W] = 1;
	last_tick_time = GetTickCount();
}
double Wheelchair::getDeltaAngle() {
	return 0.0;
}

double Wheelchair::getDeltaPosition() {
	return 0.0;
}

void Wheelchair::setPositionAndOrientation(q_vec_type pos, q_type quat) {
	q_vec_copy(my_pos_and_quat.xyz, pos);
	q_copy(my_pos_and_quat.quat, quat);
}

void Wheelchair::getPositionAndOrientation(q_vec_type pos, q_type quat) {
	DWORD new_tick_time = GetTickCount();
	double delta_time = new_tick_time - last_tick_time;
	if (delta_time > MIN_DELTA_TIME) {
		//std::cout << "Delta time: " << delta_time << "\n";
		updatePositionAndRotation();
		last_tick_time = new_tick_time;
		//std::cout << "POS  { " << my_pos_and_quat.xyz[0] << ", " << my_pos_and_quat.xyz[1] << ", " << my_pos_and_quat.xyz[2] << " }\n";
		//std::cout << "QUAT { " << my_pos_and_quat.quat[0] << ", " << my_pos_and_quat.quat[1] << ", " << my_pos_and_quat.quat[2] << ", " << my_pos_and_quat.quat[3] << " }\n";
	}	
	q_vec_copy(pos, my_pos_and_quat.xyz);
	q_copy(quat, my_pos_and_quat.quat);
}

void Wheelchair::updatePositionAndRotation() {

	double delta_time = getDeltaTime();
	double position_scale = 0.06;
	double angle_scale = 0.04;

	//update position:
	double delta_position = getDeltaPosition() * position_scale * delta_time;
	if (delta_position != 0.0) {
#ifdef WIIMOTE_RIGHT_HAND_COORDINATES
		q_vec_type delta_vector = { -delta_position, 0, 0 };
#else
		q_vec_type delta_vector = { 0, 0, delta_position };
#endif
		q_type inverse;
		q_invert(inverse, my_pos_and_quat.quat);
		q_xform(delta_vector, inverse, delta_vector);
		q_vec_add(my_pos_and_quat.xyz, my_pos_and_quat.xyz, delta_vector);
	}

	//update quaternion:
	double delta_angle = getDeltaAngle() * angle_scale * delta_time;
	if (delta_angle != 0.0) {
		q_type delta_quat;
		q_from_axis_angle(delta_quat, 0.0, 1.0, 0.0, Q_DEG_TO_RAD(delta_angle)); //rotate around Y-axis
		q_mult(my_pos_and_quat.quat, delta_quat, my_pos_and_quat.quat);
		q_normalize(my_pos_and_quat.quat, my_pos_and_quat.quat);
	}
}

double Wheelchair::getDeltaTime() {
	DWORD new_tick_time = GetTickCount();
	double delta_time = new_tick_time - last_tick_time;
	return delta_time;
}

/* Methods for class WiiMoteWheelchair */

WiiMoteWheelchair::~WiiMoteWheelchair() {
	wiimote = NULL;
}

WiiMoteWheelchair::WiiMoteWheelchair(WiiMoteClient *client) {
	wiimote = client;
}

void WiiMoteWheelchair::setWiiMote(WiiMoteClient *client) {
	if (wiimote) {
		delete wiimote;
	}
	wiimote = client;
}

void WiiMoteWheelchair::updatePositionAndRotation() {
	if (wiimote) {
		wiimote->updateFromServer();
	}
	Wheelchair::updatePositionAndRotation();
}

void WiiMoteWheelchair::rumble(bool rumble) {
	if (wiimote) {
		wiimote->enable_rumble(rumble);
	}
}

/* Methods for class ButtonWheelchair */

ButtonWheelchair::ButtonWheelchair(WiiMoteClient *client) : WiiMoteWheelchair(client) {
}

ButtonWheelchair::~ButtonWheelchair() {
	WiiMoteWheelchair::~WiiMoteWheelchair();
}

double ButtonWheelchair::getDeltaPosition() {
	double delta_position = 0.0;
	if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_UP]) {
		delta_position = 0.5;
	} else if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_DOWN]) {
		delta_position = -0.5;
	}
	return delta_position;
}

double ButtonWheelchair::getDeltaAngle() {
	double delta_angle = 0.0;
	if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_LEFT]) {
		delta_angle = -1.0;
	} else if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_RIGHT]) {
		delta_angle = 1.0;
	}
	if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_DOWN]) {
		//user says they should be turning that direction.
		delta_angle = -delta_angle;
	}
	return delta_angle;
}

void ButtonWheelchair::updatePositionAndRotation() {
	WiiMoteWheelchair::updatePositionAndRotation();

	//revert position/quat state: (should be disabled when a real user is testing the system)
	if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_HOME]) {
		q_vec_set(my_pos_and_quat.xyz, 0, 0, 0);
		my_pos_and_quat.quat[Q_X] = 0;
		my_pos_and_quat.quat[Q_Y] = 0;
		my_pos_and_quat.quat[Q_Z] = 0;
		my_pos_and_quat.quat[Q_W] = 1;
	}
}

/* Methods for class JoystickWheelchair */

JoystickWheelchair::JoystickWheelchair(WiiMoteClient *client) :
	WiiMoteWheelchair(client) {}

JoystickWheelchair::~JoystickWheelchair() {
	WiiMoteWheelchair::~WiiMoteWheelchair();
}

void JoystickWheelchair::updatePositionAndRotation() {
	WiiMoteWheelchair::updatePositionAndRotation();

	//revert position/quat state: (should be disabled when a real user is testing the system)
	if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_HOME]) {
		Wheelchair::resetPositionAndOrientation();
		std::cout << "Reset!!!!\n";
	}
}
double JoystickWheelchair::getDeltaAngle() {
	double delta_angle = wiimote->joystick_pos[0];
	if (wiimote->buttons[WIIMOTE_CLIENT_BUTTON_DOWN]) {
		//user says they should be turning that direction.
		delta_angle = -delta_angle;
	}
	double drift = 0.02;
	double max = 2.0;
	if (-drift < delta_angle && delta_angle < drift) {
		delta_angle = 0.0;
	} else if (-max < delta_angle && delta_angle < max) {
		//OK
	} else { //indeterminate number
		//std::cout << "BAD angle: " << delta_angle << "\n";
		delta_angle = 0.0;
	}
	return delta_angle;
}

double JoystickWheelchair::getDeltaPosition() {
	double delta_position = wiimote->joystick_pos[1];
	double drift = 0.04;
	double max = 2.0;
	if (-drift < delta_position && delta_position < drift) {
		delta_position = 0.0;
	} else if (-max < delta_position && delta_position < max) {
		//OK
	} else { //indeterminate number
		//std::cout << "BAD position: " << delta_position << "\n";
		delta_position = 0.0;
	}
	return delta_position;
}


/** ManualWheelchairDelegate implementation */

ManualWheelchairDelegate::ManualWheelchairDelegate(WiiMoteClient *client) :
		WiiMoteWheelchair(client) {
	sample = 0;
	for (int ii = 0; ii < SAMPLE_SIZE; ii++) {
		samples[ii] = 0;
		sample_times[ii] = GetTickCount();
	}
}

ManualWheelchairDelegate::~ManualWheelchairDelegate() {
	WiiMoteWheelchair::~WiiMoteWheelchair();
}

bool is_NAN2(double num) {
	double max = 100000000;
	return !(-max < num && num < max);
}

double ManualWheelchairDelegate::getDeltaPosition() {
	double x = wiimote->wiimote_accel[0];
	double y = wiimote->wiimote_accel[1];
	double z = wiimote->wiimote_accel[2]; //ignore
	
	//pretending that there is no centripetal accel.
	double angle = (y == 0) ? 0 : atan(x / y);
	if (y < 0) {
		angle += W_PI;
	}
	if (is_NAN2(angle)) {
		std::cout << "Bad angle: " << angle << " (x=" << x << ", y=" << y << ", z=" << z << ")!\n";
	}
	samples[sample] = (y == 0) ? samples[(sample - 1 + SAMPLE_SIZE) % SAMPLE_SIZE] : angle;
	sample_times[sample] = GetTickCount();

	//double totalVeloc = 0;
	//for (int i = 0; i < SAMPLE_SIZE - 1; i++) {
	//	int sample1 = sample - i;
	//	int sample2 = (sample - i - 1 + SAMPLE_SIZE) % SAMPLE_SIZE;
	//	totalVeloc += samples[sample1] - samples[sample2];
	//}
	double delta_time = sample_times[sample] - sample_times[(sample + 1) % SAMPLE_SIZE];
	double delta_angle = samples[sample] - samples[(sample + 1) % SAMPLE_SIZE];

	if (delta_angle < -W_PI) {
		delta_angle += 2 * W_PI;
	} else if (W_PI < delta_angle) {
		delta_angle -= 2 * W_PI;
	}
	int num_samples = SAMPLE_SIZE - 1;

	//std::cout << "Delta_time: " << delta_time << ", delta_angle: " << delta_angle << "\n";
	veloc = delta_angle / delta_time * VELOC_SCALE;
	return veloc;
}

void ManualWheelchairDelegate::updateFromServer() {
	if (wiimote) {
		wiimote->updateFromServer();
	}
	sample = (sample + 1) % SAMPLE_SIZE;
}

/** ManualWheelchair Implementation */

ManualWheelchair::ManualWheelchair(WiiMoteClient *client1, WiiMoteClient *client2) {
	leftWheel  = new ManualWheelchairDelegate(client1);
	rightWheel = new ManualWheelchairDelegate(client2);
}

ManualWheelchair::~ManualWheelchair() {
	delete leftWheel;
	delete rightWheel;
}

void ManualWheelchair::rumble(bool rumble) {
	if (leftWheel) {
		leftWheel->rumble(rumble);
	}
	if (rightWheel) {
		rightWheel->rumble(rumble);
	}
}

void ManualWheelchair::updatePositionAndRotation() {
	if (leftWheel) {
		leftWheel->updateFromServer();
		deltaPositionLeft = leftWheel->getDeltaPosition();
		//std::cout << "Delta pos (left):  " << deltaPositionLeft << "\n";
	} else {
		deltaPositionLeft = 0;
	}
	if (rightWheel) {
		rightWheel->updateFromServer();
		deltaPositionRight = rightWheel->getDeltaPosition();
		//std::cout << "Delta pos (right): " << deltaPositionRight << "\n";
	} else {
		deltaPositionRight = 0;
	}
	Wheelchair::updatePositionAndRotation();
}

double ManualWheelchair::getDeltaPosition() {
	//gets the right direction, but not quite right (might need to move less, and turn more).
	return 10 * (deltaPositionLeft - deltaPositionRight);
}

double ManualWheelchair::getDeltaAngle() {
	return 4 * (deltaPositionRight + deltaPositionLeft);
}

void ManualWheelchair::swapWheels() {
	std::cout << "Swapping Manual wheelchair wheels.\n";
	ManualWheelchairDelegate *tmp = leftWheel;
	leftWheel = rightWheel;
	rightWheel = tmp;
}