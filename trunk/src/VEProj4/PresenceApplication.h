#pragma once

#include "ExampleApplication.h"
#include "ExampleFrameListener.h"
#include "model_manager.h"
#include "GestureFSM.h"
#include "wiimote_client.h"
#include "WiiMoteGesturer.h"

#include "CEGUI/CEGUI.h"
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
#include "OgreCEGUITexture.h"

#define NUM_OBJMODELS (15)

class PresenceApplication : public ExampleApplication
{
public:
    PresenceApplication(char*, char*);
	~PresenceApplication();

	char* buttonobjModels[NUM_OBJMODELS];	

private:
	CEGUI::OgreCEGUIRenderer* mGUIRenderer;
	CEGUI::System* mGUISystem;
	CEGUI::Window* mEditorGuiSheet;
	CEGUI::Window* mPreview; // StaticImage
	CEGUI::Window* mTip;
	typedef std::map<CEGUI::String, CEGUI::String> DescriptionMap;
	DescriptionMap mDescriptionMap;
	

protected:
	
	GestureFSM *fsm;
	ModelManager *model_manager;
	SceneNode* mainSceneNode;
	RenderWindow *mWindow2;
	WiiMoteClient *wiimote;  //IR tracker
	WiiMoteClient *nunchuk;  //nunchuk (for lookaround)
	WiiMoteGesturer *gesturer;
	

    // Just override the mandatory create scene method
    void createScene(void);

    // Create new frame listener
    void createFrameListener(void);

	void createCamera(void);

	void chooseSceneManager(void) {
        // Create the SceneManager, in this case a generic one
		mSceneMgr = mRoot->createSceneManager("OctreeSceneManager");
    }

	// GUI stuff
	void setupEventHandlers(void);
	void loadMenuItems(int numberOfObjects);

	bool handleModels(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

		wmgr.getWindow("Menu")->setVisible(true);
		wmgr.getWindow("Models")->setVisible(false);
		wmgr.getWindow("CameraButton")->setVisible(false);
		wmgr.getWindow("Trash")->setVisible(false);
		wmgr.getWindow("Undo")->setVisible(false);

		return true;
	}
	bool handleCancel(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		wmgr.getWindow("Menu")->setVisible(false);
		wmgr.getWindow("Models")->setVisible(true);
		wmgr.getWindow("CameraButton")->setVisible(true);
		wmgr.getWindow("Trash")->setVisible(true);
		wmgr.getWindow("Undo")->setVisible(true);
		return true;
	}
	bool handleScreenshot(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		//wmgr.getWindow("Models")->setVisible(false);
		//wmgr.getWindow("Screenshot")->setVisible(false);
		//wmgr.getWindow("Trash")->setVisible(false);
		//wmgr.getWindow("Menu")->setVisible(false);
		//CEGUI::MouseCursor::getSingleton().hide();
		((ExampleFrameListener*) mFrameListener)->screenCapture();
		//CEGUI::MouseCursor::getSingleton().show();
		//wmgr.getWindow("Models")->setVisible(true);
		//wmgr.getWindow("Screenshot")->setVisible(true);
		//wmgr.getWindow("Trash")->setVisible(true);
		

		return true;

	}
	bool handleTrash(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		fsm->delete_selection();
		return true;
	}

	bool handleMouseEnters(const CEGUI::EventArgs& e)
	{
		CEGUI::WindowEventArgs& we = ((CEGUI::WindowEventArgs&)e);
		DescriptionMap::iterator i = mDescriptionMap.find(we.window->getName());
		if (i != mDescriptionMap.end())
		{
			mTip->setText(i->second);
		}
		else
		{
			mTip->setText((CEGUI::utf8*)"");
		}

		return true;
	}
	bool handleMouseLeaves(const CEGUI::EventArgs& e)
	{
		mTip->setText((CEGUI::utf8*)"");
		return true;
	}
	void setupEnterExitEvents(CEGUI::Window* win)
	{
		win->subscribeEvent(
			CEGUI::Window::EventMouseEnters, 
			CEGUI::Event::Subscriber(&PresenceApplication::handleMouseEnters, this));
		win->subscribeEvent(
			CEGUI::Window::EventMouseLeaves, 
			CEGUI::Event::Subscriber(&PresenceApplication::handleMouseLeaves, this));
		for (unsigned int i = 0; i < win->getChildCount(); ++i)
		{
			CEGUI::Window* child = win->getChildAtIdx(i);
			setupEnterExitEvents(child);
		}

	}

	bool handleMenuObjects(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		wmgr.getWindow("Menu")->setVisible(false);
		wmgr.getWindow("Models")->setVisible(true);
		wmgr.getWindow("CameraButton")->setVisible(true);
		wmgr.getWindow("Trash")->setVisible(true);
		wmgr.getWindow("Undo")->setVisible(true);

		CEGUI::WindowEventArgs& we = ((CEGUI::WindowEventArgs&)e);
		CEGUI::String buttonName = we.window->getName();
		std::cout<<buttonName<<std::endl;

		for(int i=1;i<=NUM_OBJMODELS;i++){
			String guiObjectName = "MenuButton" + StringConverter::toString(i);
			if((CEGUI::utf8*)guiObjectName.c_str() == buttonName){
				fsm->create_model(buttonobjModels[i-1],Vector3(-4,0,11));
			}
		}

		return true;
	}

	bool handleUndo(const CEGUI::EventArgs& e){
		return true;
	}	
	

};

