#pragma once
/*
-----------------------------------------------------------------------------
Filename:    KeyboardGestureDriver.h
Description: Defines an gesture driver which responds to frame events.
I:  Insert low-polygon tree.
B:	Begin translation
When translation is active:
	Backspace:	Cancel translation
	Enter:	Commit translation
	W/A/S/D: Move tree around X-Z plane.
-----------------------------------------------------------------------------
*/

#include "gestures.h"
#include "model_manager.h"
#include "ExampleFrameListener.h"

using namespace Ogre;

class KeyboardGestureDriver : public ExampleFrameListener, public GestureDriver {
private:
	//A finite-state-machine for placing low-poly models into the scene. :)
	bool created;
	SimpleModel *model;
	bool translating;
	double delta[3];
	double delta_delta;
protected:
	ModelManager *model_manager;
	//virtual void updateStats(void) {}
public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	KeyboardGestureDriver(ModelManager *callback, RenderWindow* win, Camera* cam) :
			ExampleFrameListener(win, cam), GestureDriver(callback) {
		showDebugOverlay(false);
		model_manager = callback;
		created = false;
		translating = false;
		delta_delta = 6;
		for (int ii = 0; ii < 3; ii++) {
			delta[ii] = 0;
		}
	}

	virtual void update_gestures() {
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt) {

		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
			return false;

		if (mKeyboard->isKeyDown(OIS::KC_I) && !created) {
			model = model_manager->placeModel("tree_low_poly.mesh", Vector3(17, 3, -28));
			created = true;
		}
		if (mKeyboard->isKeyDown(OIS::KC_B) && created && !translating) {
			translating = true;
			callback->translate_started(model);
		}

		if (translating) {
			for (int ii = 0; ii < 3; ii++) {
				delta[ii] = 0;
			}
			//TODO : re-orient delta according to camera's orientation.
			if (mKeyboard->isKeyDown(OIS::KC_A)) { // Z
				delta[2] += delta_delta * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_S)) { // X
				delta[0] += delta_delta * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) { // -Z
				delta[2] -= delta_delta * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_W)) { // -X
				delta[0] -= delta_delta * evt.timeSinceLastFrame;
			}

			if (mKeyboard->isKeyDown(OIS::KC_RETURN)) {
				callback->translate_finished();
				translating = false;
				created = false; //allow more objects to be placed...
			} else if (mKeyboard->isKeyDown(OIS::KC_BACK)) {
				translating = false;
				callback->translate_cancelled();
			} else {
				callback->translate_update(Vector3(delta[0], delta[1], delta[2]));
			}
		} else {
			return ExampleFrameListener::processUnbufferedKeyInput(evt);
		}
		return true;
	}
};
