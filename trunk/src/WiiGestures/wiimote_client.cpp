#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "wiimote_client.h"

#define PI 3.1415926

void VRPN_CALLBACK analog_change_callback(void *userdata, const vrpn_ANALOGCB info) {
	((WiiMoteClient *)userdata)->handle_analog_changes(info);
};
void VRPN_CALLBACK button_change_callback(void *userdata, const vrpn_BUTTONCB info) {
	((WiiMoteClient *)userdata)->handle_button_changes(info);
};

bool WiiMoteClient::update_channel(const vrpn_int32 channel, const bool value) {
	vrpn_float64 channel_val = (value ? 1.0 : 0.0);
	return output_listener->request_change_channel_value(channel, channel_val);
}
bool WiiMoteClient::enable_rumble(bool rumble) {
	return update_channel(VRPN_WIIMOTE_CHANNEL_RUMBLE, rumble);
}
bool WiiMoteClient::set_mode(const int mode, bool value) {
	switch (mode) {
	case VRPN_WIIMOTE_CHANNEL_MODE_IR:
	case VRPN_WIIMOTE_CHANNEL_MODE_MOTION_SENSE:
		return update_channel(mode, value);
	default:
		return false;
	}
}

bool WiiMoteClient::set_leds(int leds) {
	vrpn_int32 channel = 0;
	switch (leds) {
		case 1:
			channel = VRPN_WIIMOTE_CHANNEL_LED_1;
			break;
		case 2:
			channel = VRPN_WIIMOTE_CHANNEL_LED_2;
			break;
		case 3:
			channel = VRPN_WIIMOTE_CHANNEL_LED_3;
			break;
		case 4:
			channel = VRPN_WIIMOTE_CHANNEL_LED_4;
			break;
		default:
			return false;
	}
	std::cout << "LEDS: " << leds << " (channel=" << channel << ")\n";
	return update_channel(channel, true);
}

WiiMoteClient::WiiMoteClient(const char *name, unsigned which, vrpn_Connection *c) {
	analog_listener = new vrpn_Analog_Remote(name, c);
	button_listener = new vrpn_Button_Remote(name, c);
	output_listener = new vrpn_Analog_Output_Remote(name, c);
	wiimote_index = which;
	
	analog_listener->register_change_handler((void *) this, analog_change_callback);
	button_listener->register_change_handler((void *) this, button_change_callback);
	for (int ii = 0; ii < 3; ii++) {
		this->nunchuk_accel[ii] = 0;
	}
	for (int ii = 0; ii < 2; ii++) {
		this->joystick_pos[ii] = 0;
	}
	for (int ii = 0; ii < 4; ii++) {
		this->ir_x[ii]    = -1;
		this->ir_y[ii]    = -1;
		this->ir_size[ii] = -1;
	}
	for (int ii = 0; ii < WIIMOTE_CLIENT_NUM_BUTTONS; ii++) {
		this->buttons[ii] = 0;
	}
	this->last_button = -1;
}

WiiMoteClient::~WiiMoteClient() {
	disconnect();
}

void WiiMoteClient::updateFromServer() {
	analog_listener->mainloop();
	button_listener->mainloop();
	output_listener->mainloop();
}

void WiiMoteClient::disconnect() {
	analog_listener->unregister_change_handler((void *)this, analog_change_callback);
	button_listener->unregister_change_handler((void *)this, button_change_callback);

	if (analog_listener) {
		delete analog_listener;
	}
	if (button_listener) {
		delete button_listener;
	}
	if (output_listener) {
		delete output_listener;
	}
}

/*
// The Analogs are in a random order, both from the primary controller:
//    channel[0] = battery level (0-1)
//    channel[1] = gravity X vector calculation (1 = Earth gravity)
//    channel[2] = gravity Y vector calculation (1 = Earth gravity)
//    channel[3] = gravity Z vector calculation (1 = Earth gravity)
//    channel[4] = X of first sensor spot (0-1023, -1 if not seen)
//    channel[5] = Y of first sensor spot (0-767, -1 if not seen)
//    channel[6] = size of first sensor spot (0-15, -1 if not seen)
//    channel[7] = X of second sensor spot (0-1023, -1 if not seen)
//    channel[9] = Y of second sensor spot (0-767, -1 if not seen)
//    channel[9] = size of second sensor spot (0-15, -1 if not seen)
//    channel[10] = X of third sensor spot (0-1023, -1 if not seen)
//    channel[11] = Y of third sensor spot (0-767, -1 if not seen)
//    channel[12] = size of third sensor spot (0-15, -1 if not seen)
//    channel[13] = X of fourth sensor spot (0-1023, -1 if not seen)
//    channel[14] = Y of fourth sensor spot (0-767, -1 if not seen)
//    channel[15] = size of fourth sensor spot (0-15, -1 if not seen)
// and on the secondary controllers (skipping values to leave room for expansion)
//    channel[16] = nunchuck gravity X vector
//    channel[17] = nunchuck gravity Y vector
//    channel[18] = nunchuck gravity Z vector
//    channel[19] = nunchuck joystick angle
//    channel[20] = nunchuck joystick magnitude
*/
void WiiMoteClient::handle_analog_changes(const vrpn_ANALOGCB info) {
	//Wiimote accelerometer data:
	wiimote_accel[0] = info.channel[1]; //TODO : replace constants with WIIMOTE_CLIENT_CHANNEL_xxx
	wiimote_accel[1] = info.channel[2];
	wiimote_accel[2] = info.channel[3];

	//IR data: (X, Y, size)
	ir_x[0]    = info.channel[4];
	ir_y[0]    = info.channel[5];
	ir_size[0] = info.channel[6];
	ir_x[1]    = info.channel[7];
	ir_y[1]    = info.channel[8];
	ir_size[1] = info.channel[9];
	ir_x[2]    = info.channel[10];
	ir_y[2]    = info.channel[11];
	ir_size[2] = info.channel[12];
	ir_x[3]    = info.channel[13];
	ir_y[3]    = info.channel[14];	
	ir_size[3] = info.channel[15];

	//Nunchuk accelerometer data:
	nunchuk_accel[0] = info.channel[16];
	nunchuk_accel[1] = info.channel[17];
	nunchuk_accel[2] = info.channel[18];

	//Nunchuk joystick position:
	vrpn_float64 joystick_angle = 90.0 - info.channel[19]; //convert to rectangular-coordinates.
	vrpn_float64 joystick_magnitude = info.channel[20];
	joystick_pos[0] = cos(joystick_angle * PI / 180.0) * joystick_magnitude;
	joystick_pos[1] = sin(joystick_angle * PI / 180.0) * joystick_magnitude;
}

/*
// The buttons are as read from the bit-fields of the primary controller (bits 0-15)
//  and then a second set for any extended controller (nunchuck bits 16-31),
//  (classic controller bits 32-47), (guitar hero 3 bits 48-63).
Wiimote:
	0 => 2
	1 => 1
	2 => B
	3 => A
	4 => -
	7 => Home
	8 => left
	9 => right
	10 => down
	11 => up
	12 => +
Nunchuk:
	16 => Z
	17 => C
	Note: buttons 5, 6, 13, 14, and 15 are not used.
*/
void WiiMoteClient::handle_button_changes(const vrpn_BUTTONCB info) {
	bool is_release = !this->buttons[info.button] && (info.state == 1);
	//TODO : Make it easier to get only button "presses" and "releases".
	//For example, feedback to wiimote "server" after certain button releases:
	if (is_release) {
		switch (info.button) {
			case WIIMOTE_CLIENT_BUTTON_A:
				this->enable_rumble(true);
				break;
			case WIIMOTE_CLIENT_BUTTON_B:
				this->enable_rumble(false);
				break;
			case WIIMOTE_CLIENT_BUTTON_1:
				this->set_leds(1);
				break;
			case WIIMOTE_CLIENT_BUTTON_2:
				this->set_leds(2);
				break;
		}
	}
	this->buttons[info.button] = (info.state == 1) ? true : false;
	this->last_button = info.button;
}

void ShowNunchuk(WiiMoteClient &client) {
	std::cout << "NUNCHUK [" << client.wiimote_index << "]:\n";
	std::cout << "Accel: { " << client.nunchuk_accel[0] << ", " << client.nunchuk_accel[1] << ", " << client.nunchuk_accel[2] << " }\n";
	std::cout << "Position: { " << client.joystick_pos[0] << ", " << client.joystick_pos[1] << " }\n";
	std::cout << "\n";
}

void ShowButtons(WiiMoteClient &client) {
	if (0 <= client.last_button && client.last_button < WIIMOTE_CLIENT_NUM_BUTTONS) {
		std::cout << "BUTTON CHANGES [" << client.wiimote_index << "]:\n";
		bool on = client.buttons[client.last_button];
		std::cout << "Button #" << client.last_button << " is " << (on ? "on" : "off") << "\n";
		std::cout << "\n";
	}
}
