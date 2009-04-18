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
	"tree_amarelo.mesh",     //origin is off
	"tree_magnolia.mesh",      //needs non-unicode material/texture names. - replaces "arvore"
	"tree_bamboo.mesh",
	"tree_cabbagepalm.mesh",
	"tree_palm.mesh",
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
#define NUM_ACTIONS 10
#define TRANSLATE_LEFT  0
#define TRANSLATE_RIGHT 1
#define TRANSLATE_UP    2
#define TRANSLATE_BACK  3
#define ROTATE_LEFT     4
#define ROTATE_RIGHT    5
#define TURN_LEFT       6
#define TURN_RIGHT      7
#define MOVE_FORWARD    8
#define MOVE_BACKWARD   9

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
	bool init;
protected:
	Vector3 defaultPosition;
	GestureFSM *fsm;
	ModelManager *model_manager;
public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	KeyboardGestureDriver(ModelManager *manager, RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer, WiiMoteClient *nunchuk) :
			HeadTrackerFrameListener(win, cam, nunchuk), mGUIRenderer(renderer), mShutdownRequested(false),
			defaultPosition(-4, 0, 11), angle(0),
			delta_delta(5), delta_angle(Degree(45)),
			init(true) {
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
			if (actions[TRANSLATE_UP]) { // Z
				delta[2] += delta_delta * evt.timeSinceLastFrame;
			}
			if (actions[TRANSLATE_LEFT]) { // X
				delta[0] += delta_delta * evt.timeSinceLastFrame;
			}
			if (actions[TRANSLATE_BACK]) { // -Z
				delta[2] -= delta_delta * evt.timeSinceLastFrame;
			}
			if (actions[TRANSLATE_RIGHT]) { // -X
				delta[0] -= delta_delta * evt.timeSinceLastFrame;
			}
			model_manager->translate_update(delta_vector);
		} else if (fsm->is_rotating()) {
			angle = Radian(0);
			if (actions[ROTATE_LEFT]) { // around Y
				angle = delta_angle * evt.timeSinceLastFrame;
			}
			if (actions[ROTATE_RIGHT]) { // around -Y
				angle = -delta_angle * evt.timeSinceLastFrame;
			}
			model_manager->rotate_update(angle);
		}
		if (actions[MOVE_FORWARD]) {
			Vector3 delta_pos = mCamera->getOrientation() * Vector3(0, 0, -delta_delta * evt.timeSinceLastFrame);
			mCamera->move(delta_pos);
		} else if (actions[MOVE_BACKWARD]) {
			Vector3 delta_pos = mCamera->getOrientation() * Vector3(0, 0, delta_delta * evt.timeSinceLastFrame);
			mCamera->move(delta_pos);
		}
		if (actions[TURN_LEFT]) {
			angle = Radian(delta_angle * evt.timeSinceLastFrame);
			mCamera->yaw(angle);
		} else if (actions[TURN_RIGHT]) {
			angle = Radian(-delta_angle * evt.timeSinceLastFrame);
			mCamera->yaw(angle);
		}
		return val;
	}

	virtual void update_gestures() {}

	void createModel(char *meshName) {
		//TODO : place new models to the fore/left of the camera.
		// I'd attach an (invisible) object to the camera, then "getPosition" whenever we need to place a new model".
		fsm->create_model(meshName, defaultPosition);
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt) {
		if (mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q)) //quit
			return false;

		for (int ii = 0; ii < NUM_MODELS; ii++) { //create model, start translating (menu-driven)
			if (mKeyboard->isKeyDown(keys_to_trees[ii])) {
				createModel(models[ii]);
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
				actions[TRANSLATE_UP] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_A)) {
				actions[TRANSLATE_LEFT] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_S)) {
				actions[TRANSLATE_BACK] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) {
				actions[TRANSLATE_RIGHT] = true;
			}
		} else if (fsm->is_rotating()) {
			if (mKeyboard->isKeyDown(OIS::KC_A)) {
				actions[ROTATE_LEFT] = true;
			}
			if (mKeyboard->isKeyDown(OIS::KC_D)) {
				actions[ROTATE_RIGHT] = true;
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
		if (init) { //Stupid HACK... but it works. See: http://www.ogre3d.org/forums/viewtopic.php?t=30125
			CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(arg.state.X.abs, arg.state.Y.abs));
			init = false;
		}
		std::cout << "Mouse: { " << arg.state.X.abs << ", " << arg.state.Y.abs << " }, relative={ " << arg.state.X.rel << ", " << arg.state.Y.rel << " }\n";
		CEGUI::System::getSingleton().injectMouseMove( arg.state.X.rel, arg.state.Y.rel );
		return true;
	}
	/** x and y are values in the range [0, 1] */
	void mouseSelection(double x, double y) {
		// Try to do a raycast from the mouse. TODO: Should be changed to the red pointer instead?
		Ogre::Ray ray = mCamera->getCameraToViewportRay(x, y);

		// go through all the intersected objects, and only highlight the bounding box of the first object
		RaySceneQuery* raySceneQuery = mCamera->getSceneManager()->createRayQuery(ray);
		RaySceneQueryResult::iterator it;
		RaySceneQueryResult& qryResult=raySceneQuery->execute();
		// iterate through the objects and only select the first one
		for( it = qryResult.begin();it!=qryResult.end();it++) {
			if (Ogre::StringUtil::startsWith(it->movable->getName(), "tree") || Ogre::StringUtil::startsWith(it->movable->getName(), "plant")) { // tree or plant
				it->movable->getParentSceneNode()->showBoundingBox(true);
				SceneNode *selectedNode = it->movable->getParentSceneNode();
				fsm->select_node(selectedNode);
				std::cout << "Found a node " << it->movable->getName() << "\n";
				break;
			}
		}
	}
	void checkMouseSelection() {
		CEGUI::Vector2 mouse = CEGUI::MouseCursor::getSingleton().getPosition();
		const OIS::MouseState &ms = mMouse->getMouseState();
		double x = Ogre::Real( mouse.d_x ) / ms.width;
		double y = Ogre::Real( mouse.d_y ) / ms.height;
		std::cout << "MousePressed: { " << x << ", " << y << " }\n";
		mouseSelection(x, y);
	}
	void injectMouseButtonPressed(int buttonID) {
		switch (buttonID) {
		case 1: // From the nunchuk Z-button
			std::cout << "Left-Click!!!\n";
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MouseButton::LeftButton);
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MouseButton::LeftButton);
			checkMouseSelection();
			break;
		case 2: // From the nunchuk C-button
			std::cout << "Right-Click!!!\n";
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MouseButton::RightButton);
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MouseButton::RightButton);
			break;
		}
	}
	//----------------------------------------------------------------//
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
		CEGUI::System::getSingleton().injectMouseButtonDown(convertOISMouseButtonToCegui(id));
		checkMouseSelection();
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
				createModel(models[ii]);
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
		if (key == OIS::KC_W) {
			actions[TRANSLATE_UP]    = down;
		} else if (key == OIS::KC_A) {
			actions[TRANSLATE_LEFT]  = down;
			actions[ROTATE_LEFT]     = down;
		} else if (key == OIS::KC_S) {
			actions[TRANSLATE_BACK]  = down;
		} else if (key == OIS::KC_D) {
			actions[TRANSLATE_RIGHT] = down;
			actions[ROTATE_RIGHT]    = down;
		} else if (key == OIS::KC_LEFT) {
			actions[TURN_LEFT]       = down;
		} else if (key == OIS::KC_RIGHT) {
			actions[TURN_RIGHT]      = down;
		} else if (key == OIS::KC_UP) {
			actions[MOVE_FORWARD]    = down;
		} else if (key == OIS::KC_DOWN) {
			actions[MOVE_BACKWARD]   = down;
		}
	}
};
