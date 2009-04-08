#pragma once

#include "ExampleApplication.h"
#include "ExampleFrameListener.h"

#include "ModelEmitter.h"

class PresenceApplication : public ExampleApplication
{
public:
    PresenceApplication(char*, char*);
	~PresenceApplication();


protected:

	SceneNode* mainSceneNode;
	RenderWindow *mWindow2;
	char* wiimote1id;
	char* wiimote2id;

    // Just override the mandatory create scene method
    void createScene(void);

    // Create new frame listener
    void createFrameListener(void);

	 void createCamera(void);

	void chooseSceneManager(void)
    {
        // Create the SceneManager, in this case a generic one
		mSceneMgr = mRoot->createSceneManager("OctreeSceneManager");
		
    }
};

