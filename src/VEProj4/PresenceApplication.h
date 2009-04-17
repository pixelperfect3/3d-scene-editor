#pragma once

#include "ExampleApplication.h"
#include "ExampleFrameListener.h"
#include "model_manager.h"
#include "wiimote_client.h"
#include "WiiMoteGesturer.h"

#include "CEGUI/CEGUI.h"
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
#include "OgreCEGUITexture.h"

class PresenceApplication : public ExampleApplication
{
public:
    PresenceApplication(char*, char*);
	~PresenceApplication();


private:
	CEGUI::OgreCEGUIRenderer* mGUIRenderer;
	CEGUI::System* mGUISystem;
	CEGUI::Window* mEditorGuiSheet;
	CEGUI::Window* mPreview; // StaticImage
	

protected:
	ModelManager *model_manager;
	SceneNode* mainSceneNode;
	RenderWindow *mWindow2;
	WiiMoteClient *wiimote;  //tracker
	WiiMoteClient *nunchuk;  //nunchuk (for lookaround)
	WiiMoteGesturer *gesturer;
	

    // Just override the mandatory create scene method
    void createScene(void);

    // Create new frame listener
    void createFrameListener(void);

	 void createCamera(void);

	void chooseSceneManager(void)
    {
        // Create the SceneManager, in this case a generic one
		mSceneMgr = mRoot->createSceneManager("OctreeSceneManager");
		model_manager = new ModelManager(mSceneMgr);
    }

	// GUI stuff
	void setupEventHandlers(void);

	bool handleModels(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

		wmgr.getWindow("Menu")->setVisible(true);
		return true;
	}
	bool handleCancel(const CEGUI::EventArgs& e){
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

		wmgr.getWindow("Menu")->setVisible(false);
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
		return true;
	}

};

