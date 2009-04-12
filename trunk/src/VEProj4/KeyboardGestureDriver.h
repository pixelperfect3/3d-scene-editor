#pragma once
/*
-----------------------------------------------------------------------------
Filename:    KeyboardGestureDriver.h
Description: Defines an gesture driver which responds to frame events.
I:  Insert low-polygon tree.
T:	Begin translation
R:	Begin rotation
When translation/rotation action is active:
	Backspace:	Cancel edit
	Enter:	Commit edit
	W/A/S/D: Move tree around X-Z plane (translation only)
	A/D: rotate around Y-axis.
-----------------------------------------------------------------------------
*/

#include "gestures.h"
#include "model_manager.h"
#include "HeadTrackerFrameListener.h"

using namespace Ogre;
		
#define NUM_TREE_MODELS (6)
#define NUM_PLANT_MODELS (5)

char *trees[NUM_TREE_MODELS] = {
	"tree_amarelo.mesh",   //nice
	"tree_bamboo.mesh",    //too green (tone it down or add texture)
	"tree_evergreen.mesh", //cheap
	"tree_finihed.mesh",   //broken? (or, just bad?)
	"tree_low_poly.mesh",  //crap
	"tree_palm.mesh"       //very nice
};
char *plants[NUM_PLANT_MODELS] = { //they all look good.
	"plant_monstera.mesh",
	"plant_octopus.mesh",
	"plant_red.mesh",
	"plant_sagopalm.mesh",
	"plant_yucca.mesh"
};

OIS::KeyCode keys_to_trees[NUM_TREE_MODELS] = {
	OIS::KC_1,
	OIS::KC_2,
	OIS::KC_3,
	OIS::KC_4,
	OIS::KC_5,
	OIS::KC_6
};

class KeyboardGestureDriver : public HeadTrackerFrameListener, public GestureDriver {
private:
	//A finite-state-machine for placing low-poly models into the scene. :)
	bool created;
	SimpleModel *model;
	bool translating;
	double delta[3];
	double delta_delta;
	bool rotating;
	Radian angle;
	Radian delta_angle;
protected:
	ModelManager *model_manager;
	//virtual void updateStats(void) {}
public:
	
	bool frameStarted(const FrameEvent &evt) {
		return HeadTrackerFrameListener::frameStarted(evt);
	}

	// Constructor takes a RenderWindow because it uses that to determine input context
	KeyboardGestureDriver(ModelManager *callback, RenderWindow* win, Camera* cam) :
			HeadTrackerFrameListener(win, cam), GestureDriver(callback) {
		showDebugOverlay(false);
		model_manager = callback;
		created = false;
		translating = false;
		rotating = false;
		delta_delta = 6;
		angle = Radian(0);
		delta_angle = Radian(Degree(30));
		for (int ii = 0; ii < 3; ii++) {
			delta[ii] = 0;
		}
	}

	virtual void update_gestures() {}


	virtual bool processUnbufferedKeyInput(const FrameEvent& evt) {
		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
			return false;

		for (int ii = 0; ii < NUM_TREE_MODELS; ii++) {
			if (mKeyboard->isKeyDown(keys_to_trees[ii]) && !created) {
				model = model_manager->placeModel(trees[ii], Vector3(-4, 0, 11));
				created = true;
				//go ahead and start translating.
				translating = true;
				callback->translate_started(model);
			}
		}
		if (mKeyboard->isKeyDown(OIS::KC_R) && created && !rotating) {
			if (translating) {
				callback->translate_finished();
				translating = false;
			}
			rotating = true;
			callback->rotate_started(model);
		}
		if (mKeyboard->isKeyDown(OIS::KC_T) && created && !translating) {
			if (rotating) {
				callback->rotate_finished();
				rotating = false;
			}
			translating = true;
			callback->translate_started(model);
		}
		if (mKeyboard->isKeyDown(OIS::KC_DELETE) && created && !translating && !rotating) {
			model_manager->remove(model);
			model = NULL;
			created = false;
		}

		if (mKeyboard->isKeyDown(OIS::KC_END) && created) { //finishes with a piece
			if (translating) {
				callback->translate_finished();
				translating = false;
			} else if (rotating) {
				callback->rotate_finished();
				rotating = false;
			}
			created = false;
		}

		if (translating) {
			for (int ii = 0; ii < 3; ii++) {
				delta[ii] = 0;
			}
			if (mKeyboard->isKeyDown(OIS::KC_W)) { // Z
				delta[2] += delta_delta * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_A)) { // X
				delta[0] += delta_delta * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_S)) { // -Z
				delta[2] -= delta_delta * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) { // -X
				delta[0] -= delta_delta * evt.timeSinceLastFrame;
			}
			//TODO : re-orient delta according to camera's orientation.
			Vector3 delta_vector(delta[0], delta[1], delta[2]);

			if (mKeyboard->isKeyDown(OIS::KC_RETURN)) {
				callback->translate_finished();
				translating = false;
			} else if (mKeyboard->isKeyDown(OIS::KC_BACK)) {
				translating = false;
				callback->translate_cancelled();
			} else {
				callback->translate_update(delta_vector);
			}
		} else if (rotating) {
			angle = Radian(0);
			if (mKeyboard->isKeyDown(OIS::KC_A)) { // Z
				angle = delta_angle * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) { // -Z
				angle = -delta_angle * evt.timeSinceLastFrame;
			}

			if (mKeyboard->isKeyDown(OIS::KC_RETURN)) {
				callback->rotate_finished();
				rotating = false;
			} else if (mKeyboard->isKeyDown(OIS::KC_BACK)) {
				rotating = false;
				callback->rotate_cancelled();
			} else {
				callback->rotate_update(angle);
			}
		} else {
			return HeadTrackerFrameListener::processUnbufferedKeyInput(evt);
		}
		return true;
	}
};
