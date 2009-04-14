#include "PresenceApplication.h"
#include "KeyboardGestureDriver.h"
#include "ogreconsole.h"

PresenceApplication::PresenceApplication(char* wiimote_name) {
	this->wiimote_name = wiimote_name;
}

PresenceApplication::~PresenceApplication() {
	delete model_manager;
}

void PresenceApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("camera");

    // Position it at 500 in Z direction
    mCamera->setPosition(Vector3(0, 3, 0));
    // Look back along -Z
    mCamera->lookAt(Vector3(0, 3, 500));
    mCamera->setNearClipDistance(1);

}

// Just override the mandatory create scene method
void PresenceApplication::createScene(void)
{

	new OgreConsole;
	OgreConsole::getSingleton().init(mRoot);
	OgreConsole::getSingleton().setVisible(false);

	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_HWRENDER_TO_TEXTURE))
    {
        // In D3D, use a 1024x1024 shadow texture
        mSceneMgr->setShadowTextureSettings(1024, 2);
    }
    else
    {
        // Use 512x512 texture in GL since we can't go higher than the window res
        mSceneMgr->setShadowTextureSettings(512, 2);
    }
    mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowFarDistance(10000);

	mSceneMgr->setAmbientLight(ColourValue(0.5,0.5,0.5,1));
	//mSceneMgr->setAmbientLight(ColourValue(0,0,0));

	Light* light = mSceneMgr->createLight("mainLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDiffuseColour(0.5,0.5,0.5);
	light->setSpecularColour(0.5,0.5,0.5);
	light->setDirection(1,-1,1	);
	light->setCastShadows(true);

	//mSceneMgr->setSkyBox(true, "Examples/CitySkybox" );

	//Plane plane;
	//plane.normal = Vector3::UNIT_Y;
	//plane.d = 0;
	//MeshManager::getSingleton().createPlane("Myplane",
	//		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
	//		30000,30000,10,10,true,1,50,50,Vector3::UNIT_Z);
	//Entity* pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	//pPlaneEnt->setMaterialName("Examples/GrassFloor");
	//pPlaneEnt->setCastShadows(false);
	//SceneNode* planeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("planeNode");
	//planeNode->translate(0,-5,0);
	//planeNode->attachObject(pPlaneEnt);
	
	mainSceneNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild("mainScene")); 
	Entity* mainSceneEn = mSceneMgr->createEntity("home","home_with_lawn.mesh");
	mainSceneNode->attachObject(mainSceneEn);
}

void PresenceApplication::createFrameListener(void)
{
	//TODO : wiimote gesture driver goes here.
	mFrameListener = new KeyboardGestureDriver(static_cast<ModelManager*>(model_manager), mWindow, mCamera);
    mRoot->addFrameListener(mFrameListener);
	//mRoot->addFrameListener(&((PresenceFrameListener*)mFrameListener)->eventMgr->mTimer);
}