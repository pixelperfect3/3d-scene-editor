#pragma once


#include "ExampleFrameListener.h"
#include "VTrak3DClient.h"

bool recenter_view = true;

//TODO : add camera panning/orbiting
Vector3 cameraOrigin(0, 2.5, 0);
double headCenter[3] = { 0, 0, 0 };

class HeadTrackerFrameListener : public ExampleFrameListener {
private:
	bool zButtonDown;
	bool cButtonDown;
protected:
	VTrak3DClient* vtrak;
	WiiMoteClient *nunchuk; // the nunchuk for navigation
public:
	HeadTrackerFrameListener(RenderWindow* win, Camera* cam, WiiMoteClient *nunchuk) : ExampleFrameListener(win, cam),
	zButtonDown(false), cButtonDown(false) {
		vtrak = new VTrak3DClient();
		if (vtrak->init("tracker", "192.168.1.39", 8900)) {   // see HandleDisplay for update loop
			std::cout << "Connected to tracker!" << std::endl;
		} else {
			std::cout << "Connection to tracker could not be made!" << std::endl;   
		}

		// set the nunchuk
		this->nunchuk = nunchuk;
	}

	~HeadTrackerFrameListener() {
		if (vtrak) {
			delete vtrak;
		}
	}
	
	virtual void injectMouseButtonPressed(int button) = 0; //Inject nunchuk button presses

	bool frameRenderingQueued(const FrameEvent& evt) {
		return ExampleFrameListener::frameRenderingQueued(evt); // Call default
	}

	// returns the delta_angle from the nunchuk
	double getDeltaAngle() {
		double delta_angle = nunchuk->joystick_pos[0];
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

	// gets the delta position from the nunchuk
	double getDeltaPosition() {
		double delta_position = nunchuk->joystick_pos[1];
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

	// The redraw frame function
	bool frameStarted(const FrameEvent &evt) {
			//TODO : "delta" head position, "delta" quaternion (use an initial reading to "center" the head).
			//TODO : smooth position/orientation values.
			//TODO : throw out orientation values that are almost "upside down", or otherwise "weird".

		bool res = ExampleFrameListener::frameStarted(evt);

		if (vtrak) { //Andrew said "vtrak->isConnected" doesn't always play nice.
			vtrak->update();
			double pos[3];
			double orient[4];

			std::string objectName = "head";
			if (vtrak->getTrackerData(objectName, pos, orient)) {
				if (recenter_view) {
					recenter_view = false;
					for (int ii = 0; ii < 3; ii++) {
						headCenter[ii] = pos[ii];
					}
				}
				
				double scale[3] = { 0.06, 0.03, 0.06 };
				double delta_pos[3];
				for (int ii = 0; ii < 3; ii++) {
					delta_pos[ii] = scale[ii] * (pos[ii] - headCenter[ii]);
				}
				Ogre::Vector3 delta_vector(-delta_pos[0], delta_pos[1], -delta_pos[2]);
				mCamera->setPosition(cameraOrigin + delta_vector);

				/* Set Orientation - Don't know whether this should happen. */
				//Ogre::Quaternion q(orient[0],-orient[1],orient[2],-orient[3]);
				//Quaternion q2(Radian(Degree(90)), Ogre::Vector3::UNIT_Y);
				//q = q2*q;

				//std::cout << "Q { " << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3] << " }\n";
				//mCamera->setOrientation(q);
			} else {
				//std::cout << "Not updating head-tracker data.\n";
			}
		} else {
			//std::cout << "Vtrak not initialized.\n";
		}

		// update the wiimote
		if (nunchuk) {
			nunchuk->updateFromServer();
			bool zButtonPressed = nunchuk->buttons[WIIMOTE_CLIENT_BUTTON_NUNCHUK_Z];
			bool cButtonPressed = nunchuk->buttons[WIIMOTE_CLIENT_BUTTON_NUNCHUK_C];

			// Change the camera position based on the nunchuk input
			float degrees_per_sec = 60;
			float deltaY = getDeltaAngle() * degrees_per_sec * evt.timeSinceLastFrame;
			Quaternion quatY(Radian(Degree(deltaY)), Vector3::NEGATIVE_UNIT_Y);
			mCamera->rotate(quatY);
			float units_per_sec = 3.5;
			float deltaZ = getDeltaPosition() * units_per_sec * evt.timeSinceLastFrame;
			Vector3 delta_pos = mCamera->getOrientation() * Vector3(0, 0, -deltaZ);
			mCamera->move(delta_pos);
			//TODO : inject nunchuk button presses.
			if (!zButtonDown && zButtonPressed) {
				injectMouseButtonPressed(1);
			}
			if (!cButtonDown && cButtonPressed) {
				injectMouseButtonPressed(2);
			}
			zButtonDown = zButtonPressed;
			cButtonDown = cButtonPressed;
		}

		return res;
	}

};