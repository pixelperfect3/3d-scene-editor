#pragma once

#include "gestures.h"
#include "ExampleApplication.h"
#include "ExampleFrameListener.h"
#include "model_manager.h"

#include "CEGUI/CEGUI.h"
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
#include "OgreCEGUITexture.h"

//#include "ModelEmitter.h"

class PresenceApplication : public ExampleApplication
{
public:
    PresenceApplication(char*);
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
	char* wiimote_name;

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
	void setupEventHandlers(void)
	{
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	}

};

