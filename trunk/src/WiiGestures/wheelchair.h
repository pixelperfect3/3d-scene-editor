#pragma once
#include <quat.h>
#include <windows.h>

#include "wiimote_client.h"

#define W_PI 3.14159265898

#define SAMPLE_SIZE (15)
#define VELOC_SCALE (50)

/**
 * A wheelchair-class object... uses quatlib for its quaternion needs.
 * Basically, any object with two degrees of freedom: velocity and direction.
 * Various subclasses exist that implement changes in either degree.
 *
 * Sample usage (in an OpenGL settings) includes:
 * <code>
 *	q_type quat = {0, 0, 0, 1};
 *	q_vec_type pos = { 0, 0, 0 };
 *	qogl_matrix_type glMatrix;
 *	wheelchair->getPositionAndOrientation(pos, quat);
 *	q_invert(quat, quat);
 *	q_to_ogl_matrix(glMatrix, quat);
 *	q_vec_invert(pos, pos);
 *	glTranslated(pos[Q_X], pos[Q_Y], pos[Q_Z]);
 *	glMultMatrixd(glMatrix);
 * </code>
 */
class Wheelchair {
public:
	Wheelchair(void);
	virtual ~Wheelchair(void);
	/**
	 * Copies the internal position vector and orientation quaternion
	 * into pos and quat, respectively.
	 */
	void getPositionAndOrientation(q_vec_type pos, q_type quat);
	/**
	 * Resets the internal position and orientation to "home".
	 * Note: should not be enabled for "live users".
	 */
	void resetPositionAndOrientation();
	void setPositionAndOrientation(q_vec_type pos, q_type quat);
	/*
	* return the pos and quat without calling update -- BY LIHAN
	*/
	q_xyz_quat_type getPositionAndOrientation(){return my_pos_and_quat;}
	virtual void rumble(bool rumble) = 0;

	virtual void swapWheels() {};

protected:
	q_xyz_quat_type my_pos_and_quat; //position vector and rotational quaternion (continually concatenated)
	/**
	 * Uses the getDeltaPosition and getDeltaAngle to update the internal position vector and quaternion.
	 */
	virtual void updatePositionAndRotation();
	/**
	 * The change in position (i.e. magnitude of the change since last polled).
	 * This is always 0.0, unless subclasses override it.
	 */
	virtual double getDeltaPosition();
	/**
	 * The change of angle, around the Y-axis, in degrees
	 * (since the last time updatePositionAndRotation was polled).
	 * This is always 0.0, unless subclasses override it.
	 */
	virtual double getDeltaAngle();
	/**
	 * @return double The delta time (since last updated/calculated), in millis.
	 */
	virtual double getDeltaTime();
private:
	DWORD last_tick_time;
};

/**
 * An abstract class wheelchair that updates a wii-remote *once* before polling for changes in position
 * and orientation. Subclasses should assume that wiiremote is up-to-date
 * when getDeltaPosition and getDeltaAngle are called.
 */
class WiiMoteWheelchair : public Wheelchair {
protected:
	WiiMoteClient *wiimote;
public:
	WiiMoteWheelchair(WiiMoteClient *client);
	~WiiMoteWheelchair();
	virtual void rumble(bool rumble);
protected:
	/**
	 * Gets the wiimote client up-to-date, then updates the position and quaterion.
	 */
	virtual void updatePositionAndRotation();
	/**
	 * Changes the wiimote client.
	 * Warning: If the old client isn't null, will destroy it.
	 */
	void setWiiMote(WiiMoteClient *client);
};

/**
 * A concrete wheelchair class that uses the D-pad of a wii-remote to determine
 * both direction and orientation. Also, the "home" button resets the position/rotation.
 */
class ButtonWheelchair : public WiiMoteWheelchair {
public:
	ButtonWheelchair(WiiMoteClient *client);
	~ButtonWheelchair();
protected:
	/* Virtual methods inherited from class Wheelchair */
	double getDeltaPosition();
	double getDeltaAngle();
	void updatePositionAndRotation();
};

/**
 * A concrete wheelchair class that uses a wii Nunchuk's joystick for direction/orientation.
 * Also, the "home" button resets the position/rotation.
 */
class JoystickWheelchair : public WiiMoteWheelchair {
public:
	JoystickWheelchair(WiiMoteClient *client);
	~JoystickWheelchair();
protected:
	/* Virtual methods inherited from class Wheelchair */
	double getDeltaPosition();
	double getDeltaAngle();
	void updatePositionAndRotation();
};

class ManualWheelchairDelegate : public WiiMoteWheelchair {
public:
	ManualWheelchairDelegate(WiiMoteClient *client);
	~ManualWheelchairDelegate();

	void updateFromServer();

	double getDeltaPosition();
protected:
	int sample;                      //index into current sample array
	double samples[SAMPLE_SIZE];     //angle of last sample, in radians
	DWORD sample_times[SAMPLE_SIZE]; // timestamp of each sample
	double veloc;                    //a (scaled) angular velocity, in radians
};

class ManualWheelchair : public Wheelchair {
public:
	ManualWheelchair(WiiMoteClient *client1, WiiMoteClient *client2);
	~ManualWheelchair();
	void rumble(bool rumble);
	
	void swapWheels();
protected:
	ManualWheelchairDelegate *leftWheel;
	ManualWheelchairDelegate *rightWheel;
	double deltaPositionLeft;  //calculated change in position, from leftWheel
	double deltaPositionRight; //calculated change in position, from rightWheel
	
	/* Virtual methods inherited from class Wheelchair */
	double getDeltaPosition();
	double getDeltaAngle();
	void updatePositionAndRotation();
};
