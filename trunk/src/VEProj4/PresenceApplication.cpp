#include "PresenceApplication.h"
#include "KeyboardGestureDriver.h"
#include "ogreconsole.h"

PresenceApplication::PresenceApplication(char* wiimote1_name, char* wiimote2_name) {
	if (wiimote2_name) {
		wiimote = new WiiMoteClient(wiimote1_name);
		nunchuk = new WiiMoteClient(wiimote2_name);
		std::cout << "Using Wiimotes \"" << wiimote1_name << "\" (for tracking) and \"" << wiimote2_name << "\" (for nunchuk).\n";
	} else if (wiimote1_name) {
		if (true) {
			wiimote = new WiiMoteClient(wiimote1_name);
			nunchuk = NULL;
			std::cout << "Using Wiimote \"" << wiimote1_name << "\" (for tracking).\n";
		} else {
			nunchuk = new WiiMoteClient(wiimote1_name);
			wiimote = NULL;
			std::cout << "Using Wiimote \"" << wiimote1_name << "\" (for nunchuk).\n";
		}
	} else {
		wiimote = NULL;
		nunchuk = NULL;
		std::cout << "Not using Wiimotes.\n";
	}
	mGUIRenderer =0;
	mGUISystem = 0;
	mEditorGuiSheet = 0;
}

PresenceApplication::~PresenceApplication() {
	delete model_manager;
	if (wiimote) {
		delete gesturer;
		delete wiimote;
	}
	if (nunchuk) {
		delete nunchuk;
	}
	if(mEditorGuiSheet)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);
	}
	if(mGUISystem)
	{
		delete mGUISystem;
		mGUISystem = 0;
	}
	if(mGUIRenderer)
	{
		delete mGUIRenderer;
		mGUIRenderer = 0;
	}
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

	// setup GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, 
		Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);

	mGUISystem = new CEGUI::System(mGUIRenderer);

	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

	// load scheme and set up defaults
	CEGUI::SchemeManager::getSingleton().loadScheme(
		(CEGUI::utf8*)"TaharezLookSkin.scheme");
	mGUISystem->setDefaultMouseCursor(
		(CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	mGUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");

	CEGUI::Window* sheet = 
		CEGUI::WindowManager::getSingleton().loadWindowLayout(
		(CEGUI::utf8*)"menu1.layout");
	mGUISystem->setGUISheet(sheet);

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	wmgr.getWindow("Menu")->setVisible(false); // menu is invisible

	setupEventHandlers();
	
	init_pointers(mSceneMgr);
}

void PresenceApplication::setupEventHandlers(){
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	wmgr.getWindow((CEGUI::utf8*)"Models")->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleModels, this));
	wmgr.getWindow((CEGUI::utf8*)"Cancel")->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&PresenceApplication::handleCancel, this));
	wmgr.getWindow((CEGUI::utf8*)"Screenshot")->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleScreenshot, this));
	wmgr.getWindow((CEGUI::utf8*)"Trash")->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleTrash, this));
}

void PresenceApplication::createFrameListener(void)
{
	KeyboardGestureDriver *driver = new KeyboardGestureDriver(model_manager, mWindow, mCamera, mGUIRenderer, nunchuk);
	mFrameListener = driver;
    mRoot->addFrameListener(mFrameListener);
	if (wiimote) {
		gesturer = new WiiMoteGesturer(wiimote, driver, model_manager);
		mRoot->addFrameListener(gesturer);
	}
}