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

		CEGUI::Window* editorWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Menu");

		CEGUI::Window* window = 0;

		Real posX = 0.2; //Math::RangeRandom(0.0, 0.0); 
		Real posY = 0.2; //Math::RangeRandom(0.0, 0.0);

		window = createRttGuiObject();

		editorWindow->addChildWindow(window);

		window->setPosition(CEGUI::UVector2(CEGUI::UDim(posX, 0), CEGUI::UDim(posY, 0)));



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

	CEGUI::Window* createRttGuiObject(void)
	{
		static unsigned int rttCounter = 0;
		String guiObjectName = "NewRttImage" + StringConverter::toString(rttCounter);

		CEGUI::Imageset* rttImageSet = 
			CEGUI::ImagesetManager::getSingleton().getImageset((CEGUI::utf8*)"RttImageset");

		CEGUI::Window* si = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/StaticImage", (CEGUI::utf8*)guiObjectName.c_str());
		si->setSize(CEGUI::UVector2( CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.4f, 0)));

		si->setProperty("Image", CEGUI::PropertyHelper::imageToString(&rttImageSet->getImage((CEGUI::utf8*)"RttImage")));

		rttCounter++;

		return si;
	}

};

