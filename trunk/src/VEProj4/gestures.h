#pragma once

#include "Ogre.h"
#include "simple_model.h"


class GestureCallback {
//start/cancel/stop each gesture, so that you can get a "live" preview of the action.
//need to "update" the current status (distance, axis, degree, size X/Y, etc.) at each frame.
public:
	virtual void translate_started(SimpleModel *o) = 0;
	virtual void translate_update(Ogre::Vector3 delta) = 0;
	virtual void translate_finished() = 0;
	virtual void translate_cancelled() = 0;

	virtual void rotate_started(SimpleModel *o) = 0;
	virtual void rotate_update(Ogre::Radian delta) = 0;
	virtual void rotate_finished() = 0;
	virtual void rotate_cancelled() = 0;

	//TODO : other gestures...
};

//select, translate, rotate, scale/delete/crush, orbiting/zooming
class GestureDriver { //Gesture-driver "interface"
protected:
	GestureCallback *callback;

public:
	GestureDriver(GestureCallback *callback) {
		this->callback = callback;
	};
	virtual ~GestureDriver() {}
	virtual void update_gestures() = 0; //once per frame
};
