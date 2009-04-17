#pragma once


#include "ExampleFrameListener.h"
#include "VTrak3DClient.h"

bool recenter_view = true;

//TODO : add camera panning/orbiting
Vector3 cameraOrigin(0, 2.5, 0);
double headCenter[3] = { 0, 0, 0 };

class HeadTrackerFrameListener : public ExampleFrameListener {
protected:
	VTrak3DClient* vtrak;
	WiiMoteClient *nunchuk;
public:
	HeadTrackerFrameListener(RenderWindow* win, Camera* cam, WiiMoteClient *nunchukN) : ExampleFrameListener(win, cam) {
		vtrak = new VTrak3DClient();
		if (vtrak->init("tracker", "192.168.1.39", 8900)) {   // see HandleDisplay for update loop
			std::cout << "Connected to tracker!" << std::endl;
		} else {
			std::cout << "Connection to tracker could not be made!" << std::endl;   
		}

		// set the nunchuk
		nunchuk = nunchukN;
	}

	~HeadTrackerFrameListener() {
		if (vtrak) {
			delete vtrak;
		}
	}

	bool frameRenderingQueued(const FrameEvent& evt) {
		return ExampleFrameListener::frameRenderingQueued(evt); // Call default
	}

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
		return res;
	}

};