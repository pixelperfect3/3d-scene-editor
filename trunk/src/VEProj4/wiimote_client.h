#pragma once

//Using a modified version of vrpn_WiiMote, enabling us to remotely set LEDs and Rumble.
//To this end, the constants VRPN_WIIMOTE_CHANNEL_LED_X were defined.

#include <vrpn/vrpn_WiiMote.h>
#include <vrpn/vrpn_Connection.h>
#include <vrpn/vrpn_Analog_Output.h>

//indices into buttons array
#define WIIMOTE_CLIENT_BUTTON_2 0
#define WIIMOTE_CLIENT_BUTTON_1 1
#define WIIMOTE_CLIENT_BUTTON_A 2
#define WIIMOTE_CLIENT_BUTTON_B 3
#define WIIMOTE_CLIENT_BUTTON_MINUS 4
#define WIIMOTE_CLIENT_BUTTON_HOME 7
#define WIIMOTE_CLIENT_BUTTON_LEFT 8
#define WIIMOTE_CLIENT_BUTTON_RIGHT 9
#define WIIMOTE_CLIENT_BUTTON_DOWN 10
#define WIIMOTE_CLIENT_BUTTON_UP 11
#define WIIMOTE_CLIENT_BUTTON_PLUS 12
#define WIIMOTE_CLIENT_BUTTON_NUNCHUK_Z 16
#define WIIMOTE_CLIENT_BUTTON_NUNCHUK_C 17

class WiiMoteClient {
public:
	WiiMoteClient(const char* name, unsigned which = 0, vrpn_Connection *c = NULL);
	~WiiMoteClient(void);
	void updateFromServer();
	void disconnect();
	void handle_analog_changes(const vrpn_ANALOGCB info);
	void handle_button_changes(const vrpn_BUTTONCB info);
	bool enable_rumble(bool rumble);
	bool set_leds(const int leds);
	bool set_mode(const int mode, bool value); //one of VRPN_WIIMOTE_CHANNEL_MODE_IR or _MODE_MOTION_SENSE
	/* (Remote) Wiimote data */
#define WIIMOTE_CLIENT_NUM_BUTTONS (64)
	unsigned wiimote_index;                   //which wiimote this is
	int last_button;                          //last button pressed or released
	bool buttons[WIIMOTE_CLIENT_NUM_BUTTONS]; //button on/off states
	double wiimote_accel[3];                  //accel in X, Y, and Z axes.
	double nunchuk_accel[3];                  //accel in X, Y, and Z axes.
	double joystick_pos[2];  //nunchuck position in X&Y axes, values (mostly) between -1 and 1.
	int ir_x[4];    // [0, 1023] or -1 if not seen
	int ir_y[4];    // [0, 767]  or -1 if not seen
	int ir_size[4]; // [0, 15]   or -1 if not seen

protected:
	bool update_channel(const int channel, const bool value);
	static vrpn_float64 get_channel_value(const bool value);
	vrpn_Analog_Remote *analog_listener; //listens for changes to thumbsticks/accel data.
	vrpn_Button_Remote *button_listener; //listens for changes to buttons.
	vrpn_Analog_Output_Remote *output_listener; //sends rumble, LED, etc. updates to wiimote.
};

void VRPN_CALLBACK analog_change_callback(void *userdata, const vrpn_ANALOGCB info);
void VRPN_CALLBACK button_change_callback(void *userdata, const vrpn_BUTTONCB info);
