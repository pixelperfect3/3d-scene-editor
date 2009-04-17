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

#include "mouseDriver.h"
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
#define NUM_ACTIONS 6
#define MOVE_LEFT  0
#define MOVE_RIGHT 1
#define MOVE_UP    2
#define MOVE_DOWN  3
#define TURN_LEFT  4
#define TURN_RIGHT 5

class KeyboardGestureDriver : public HeadTrackerFrameListener,public OIS::KeyListener, public OIS::MouseListener, public MouseDriver {
private:
	//A finite-state-machine for placing low-poly models into the scene. :)
	double delta[3];
	double delta_delta;
	Radian angle;
	Radian delta_angle;
	bool actions[NUM_ACTIONS];

	CEGUI::Renderer* mGUIRenderer;
	bool mShutdownRequested;
protected:
	Vector3 defaultPosition;
	GestureFSM *fsm;
	ModelManager *model_manager;
public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	KeyboardGestureDriver(ModelManager *manager, RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer, WiiMoteClient *nunchuk) :
			HeadTrackerFrameListener(win, cam, nunchuk), mGUIRenderer(renderer), mShutdownRequested(false),
			defaultPosition(-4, 0, 11), angle(0),
			delta_delta(5), delta_angle(Degree(45)) {
		mMouse->setEventCallback(this);
		mKeyboard->setEventCallback(this);
		showDebugOverlay(false);
		model_manager = manager;
		for (int ii = 0; ii < 3; ii++) {
			delta[ii] = 0;
		}
		for (int ii = 0; ii < NUM_ACTIONS; ii++) {
			actions[ii] = false;
		}
		fsm = new GestureFSM(manager);
	}
	~KeyboardGestureDriver() {
		delete fsm;
	}
	bool frameStarted(const FrameEvent &evt) {
		bool val = HeadTrackerFrameListener::frameStarted(evt);
		Vector3 delta_vector(delta[0], delta[1], delta[2]);
		for (int ii = 0; ii < 3; ii++) {
			delta[ii] = 0;
		}
		if (fsm->is_translating()) {
			if (actions[MOVE_UP]) { // Z
				delta[2] += delta_delta * evt.timeSinceLastFrame;
			}
			if (actions[MOVE_LEFT]) { // X
				delta[0] += delta_delta * evt.timeSinceLastFrame;
			}
			if (actions[MOVE_DOWN]) { // -Z
				delta[2] -= delta_delta * evt.timeSinceLastFrame;
			}
			if (actions[MOVE_RIGHT]) { // -X
				delta[0] -= delta_delta * evt.timeSinceLastFrame;
			}
			Quaternion orient(mCamera->getOrientation());
			delta_vector = orient * -delta_vector;
			model_manager->translate_update(delta_vector);
		} else if (fsm->is_rotating()) {
			angle = Radian(0);
			if (actions[TURN_LEFT]) { // around Y
				angle = delta_angle * evt.timeSinceLastFrame;
			}
			if (actions[TURN_RIGHT]) { // around -Y
				angle = -delta_angle * evt.timeSinceLastFrame;
			}
			model_manager->rotate_update(angle);
		}
		return val;
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

		for (int ii = 0; ii < NUM_ACTIONS; ii++) {
			actions[ii] = false;
		}
		if (fsm->is_translating()) {
			if (mKeyboard->isKeyDown(OIS::KC_W)) {
				actions[MOVE_UP] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_A)) {
				actions[MOVE_LEFT] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_S)) {
				actions[MOVE_DOWN] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) {
				actions[MOVE_RIGHT] = true;
			}
		} else if (fsm->is_rotating()) {
			if (mKeyboard->isKeyDown(OIS::KC_A)) {
				actions[TURN_LEFT] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) {
				actions[TURN_RIGHT] = true;
			}
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
	void ToggleMouseCursor(bool useMouse) {
		if (useMouse) {
			std::cout << "Showing mouse.\n";
			CEGUI::MouseCursor::getSingleton().show();
		} else {
			std::cout << "Hiding mouse.\n";
			CEGUI::MouseCursor::getSingleton().hide();
		}
	}
	void MouseMoved(double mouse[3], double old_mouse[3], double distance[2]) {
		const OIS::MouseState &ms = mMouse->getMouseState();
		int x = old_mouse[0] * ms.width;
		int y = ms.height - old_mouse[1] * ms.height;
		int relX = distance[0] * ms.width;
		int relY = -distance[1] * ms.height;
		std::cout << "Mouse: { " << x << ", " << y << " }, relative={ " << relX << ", " << relY << " }\n";
		CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(x, y));
		CEGUI::System::getSingleton().injectMouseMove(relX, relY); //These are relative movements in pixels.
	}

	//----------------------------------------------------------------//
	bool mouseMoved( const OIS::MouseEvent &arg )
	{
		std::cout << "Mouse: { " << arg.state.X.abs << ", " << arg.state.Y.abs << " }, relative={ " << arg.state.X.rel << ", " << arg.state.Y.rel << " }\n";
		CEGUI::System::getSingleton().injectMouseMove( arg.state.X.rel, arg.state.Y.rel );
		return true;
	}
	void injectMouseButtonPressed(int buttonID) {
		switch (buttonID) {
			case 1:
				std::cout << "Left-Click!!!\n";
				CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MouseButton::LeftButton);
				CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MouseButton::LeftButton);
				break;
			case 2:
				std::cout << "Right-Click!!!\n";
				CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MouseButton::RightButton);
				CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MouseButton::RightButton);
				break;
		}
		
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
	bool keyPressed( const OIS::KeyEvent &arg ) {
		if (arg.key == OIS::KC_ESCAPE) {
			mShutdownRequested = true;
		}
		if (arg.key == OIS::KC_SYSRQ) {
			screenCapture();
		}

		for (int ii = 0; ii < NUM_MODELS; ii++) { //create model, start translating (menu-driven)
			if (arg.key == keys_to_trees[ii]) {
				fsm->create_model(models[ii], defaultPosition);
			}
		}
		if (arg.key == OIS::KC_DELETE) {
			fsm->delete_selection();
		} else if (arg.key == OIS::KC_END) {
			fsm->done_with_selection();
		} else if (arg.key == OIS::KC_RETURN) {
			fsm->done_with_move();
		} else if (arg.key == OIS::KC_BACK) {
			fsm->cancel_move();
		} else if (arg.key == OIS::KC_R) {
			fsm->start_rotating();
		} else if (arg.key == OIS::KC_T) {
			fsm->start_translating();
		}
		
		update_actions(arg.key, true);

		CEGUI::System::getSingleton().injectKeyDown( arg.key );
		CEGUI::System::getSingleton().injectChar( arg.text );
		return true;
	}

	//----------------------------------------------------------------//
	bool keyReleased( const OIS::KeyEvent &arg )
	{
		update_actions(arg.key, false);
		CEGUI::System::getSingleton().injectKeyUp( arg.key );
		return true;
	}

	void update_actions(OIS::KeyCode key, bool down) {
		if (fsm->is_translating()) {
			if (key == OIS::KC_W) {
				actions[MOVE_UP] = down;
			} else if (key == OIS::KC_A) {
				actions[MOVE_LEFT] = down;
			} else if (key == OIS::KC_S) {
				actions[MOVE_DOWN] = down;
			} else if (key == OIS::KC_D) {
				actions[MOVE_RIGHT] = down;
			}
		} else if (fsm->is_rotating()) {
			if (key == OIS::KC_A) {
				actions[TURN_LEFT] = down;
			}
			if (key == OIS::KC_D) {
				actions[TURN_RIGHT] = down;
			}
		}
	}
};
