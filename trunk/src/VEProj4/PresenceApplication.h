#pragma once

#include "gestures.h"
#include "ExampleApplication.h"
#include "ExampleFrameListener.h"
#include "model_manager.h"

//#include "ModelEmitter.h"

class PresenceApplication : public ExampleApplication
{
public:
    PresenceApplication(char*);
	~PresenceApplication();

protected:
	GestureCallback *gestureCallback;
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
		gestureCallback = new ModelManager(mSceneMgr);
    }
};

