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

#include "model_manager.h"
#include "HeadTrackerFrameListener.h"
#include "GestureFSM.h"

#include "CEGUI/CEGUI.h"


using namespace Ogre;
		
#define NUM_MODELS (10)

char *models[NUM_MODELS] = {
	"tree_amarelo.mesh",     //nice
	"tree_arvore.mesh",      
	"tree_bamboo.mesh",      //too green (tone it down or add texture)
	"tree_cabbagepalm.mesh",
	"tree_palm.mesh",         //very nice
	//all the "ground" meshes look good.
	"plant_monstera.mesh",
	"plant_octopus.mesh",
	"plant_red.mesh",
	"plant_sagopalm.mesh",
	"plant_yucca.mesh"
};

OIS::KeyCode keys_to_trees[NUM_MODELS] = {
	OIS::KC_1,
	OIS::KC_2,
	OIS::KC_3,
	OIS::KC_4,
	OIS::KC_5,
	OIS::KC_6,
	OIS::KC_7,
	OIS::KC_8,
	OIS::KC_9,
	OIS::KC_0
};

class KeyboardGestureDriver : public HeadTrackerFrameListener,public OIS::KeyListener, public OIS::MouseListener {
private:
	//A finite-state-machine for placing low-poly models into the scene. :)
	double delta[3];
	double delta_delta;
	Radian angle;
	Radian delta_angle;

	CEGUI::Renderer* mGUIRenderer;
	bool mShutdownRequested;
protected:
	Vector3 defaultPosition;
	GestureFSM *fsm;
	ModelManager *model_manager;
public:
	
	bool frameStarted(const FrameEvent &evt) {
		return HeadTrackerFrameListener::frameStarted(evt);
	}

	// Constructor takes a RenderWindow because it uses that to determine input context
	KeyboardGestureDriver(ModelManager *manager, RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer) :
			HeadTrackerFrameListener(win, cam), mGUIRenderer(renderer),mShutdownRequested(false)
			{
				mMouse->setEventCallback(this);
				mKeyboard->setEventCallback(this);
		showDebugOverlay(false);
		model_manager = manager;
		delta_delta = 5;
		delta_angle = Radian(Degree(45));
		for (int ii = 0; ii < 3; ii++) {
			delta[ii] = 0;
		}
		angle = Radian(0);
		defaultPosition = Vector3(-4, 0, 11);
		fsm = new GestureFSM(manager);
	}
	~KeyboardGestureDriver() {
		delete fsm;
	}

	virtual void update_gestures() {}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt) {
		if (mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q)) //quit
			return false;

		for (int ii = 0; ii < NUM_MODELS; ii++) { //create model, start translating (menu-driven)
			if (mKeyboard->isKeyDown(keys_to_trees[ii])) {
				fsm->create_model(models[ii], defaultPosition);
			}
		}
		if (mKeyboard->isKeyDown(OIS::KC_DELETE)) {
			fsm->delete_selection();
		} else if (mKeyboard->isKeyDown(OIS::KC_END)) {
			fsm->done_with_selection();
		} else if (mKeyboard->isKeyDown(OIS::KC_RETURN)) {
			fsm->done_with_move();
		} else if (mKeyboard->isKeyDown(OIS::KC_BACK)) {
			fsm->cancel_move();
		} else if (mKeyboard->isKeyDown(OIS::KC_R)) {
			fsm->start_rotating();
		} else if (mKeyboard->isKeyDown(OIS::KC_T)) {
			fsm->start_translating();
		}

		if (fsm->is_translating()) {
			//TODO : re-orient delta according to camera's orientation
			// (see what ExampleFrameListener does for delta position each frame).
			Vector3 delta_vector(delta[0], delta[1], delta[2]);
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
			model_manager->translate_update(delta_vector);
		} else if (fsm->is_rotating()) {
			angle = Radian(0);
			if (mKeyboard->isKeyDown(OIS::KC_A)) { // around Y
				angle = delta_angle * evt.timeSinceLastFrame;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) { // around -Y
				angle = -delta_angle * evt.timeSinceLastFrame;
			}
			model_manager->rotate_update(angle);
		} else {
			return HeadTrackerFrameListener::processUnbufferedKeyInput(evt);
		}
		return true;
	}

	CEGUI::MouseButton convertOISMouseButtonToCegui(int buttonID)
	{
		switch (buttonID)
		{
		case 0: return CEGUI::LeftButton;
		case 1: return CEGUI::RightButton;
		case 2:	return CEGUI::MiddleButton;
		case 3: return CEGUI::X1Button;
		default: return CEGUI::LeftButton;
		}
	}
	void requestShutdown(void)
	{
		mShutdownRequested = true;
	}

	bool frameEnded(const FrameEvent& evt)
	{
		if (mShutdownRequested)
			return false;
		else
			return ExampleFrameListener::frameEnded(evt);
	}
	//----------------------------------------------------------------//
	bool mouseMoved( const OIS::MouseEvent &arg )
	{
		CEGUI::System::getSingleton().injectMouseMove( arg.state.X.rel, arg.state.Y.rel );
		return true;
	}

	//----------------------------------------------------------------//
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		CEGUI::System::getSingleton().injectMouseButtonDown(convertOISMouseButtonToCegui(id));
		return true;
	}

	//----------------------------------------------------------------//
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		CEGUI::System::getSingleton().injectMouseButtonUp(convertOISMouseButtonToCegui(id));
		return true;
	}

	//----------------------------------------------------------------//
	bool keyPressed( const OIS::KeyEvent &arg )
	{
		if( arg.key == OIS::KC_ESCAPE )
			mShutdownRequested = true;
		if( arg.key == OIS::KC_S){
			std::cout << "Screen capture!\n";
			screenCapture();
		}
		CEGUI::System::getSingleton().injectKeyDown( arg.key );
		CEGUI::System::getSingleton().injectChar( arg.text );
		return true;
	}

	//----------------------------------------------------------------//
	bool keyReleased( const OIS::KeyEvent &arg )
	{
		CEGUI::System::getSingleton().injectKeyUp( arg.key );
		return true;
	}
};
