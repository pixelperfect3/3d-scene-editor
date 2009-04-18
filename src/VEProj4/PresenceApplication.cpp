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
			wiimote = NULL;
			nunchuk = new WiiMoteClient(wiimote1_name);
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
	if (nunchuk && nunchuk != wiimote) {
		delete nunchuk;
	}
	if (wiimote) {
		delete gesturer;
		delete wiimote;
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

	mSceneMgr->setSkyBox(true, "Examples/Skybox" );
	
	
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
	setupEventHandlers();
	loadMenuItems(63);
	
	init_pointers(mSceneMgr);
}

void PresenceApplication::setupEventHandlers(){
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* window = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Root");

	wmgr.getWindow("Menu")->setVisible(false); // menu is invisible
	wmgr.getWindow("Menu")->setAlpha(.8);

	wmgr.getWindow((CEGUI::utf8*)"Cancel")->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&PresenceApplication::handleCancel, this));

	CEGUI::Window* modelButton = wmgr.getWindow("Models");
	modelButton->setAlpha(.7);
	modelButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleModels, this));

	CEGUI::Window* cameraButton =CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/ImageButton", (CEGUI::utf8*)"CameraButton");
	cameraButton->setAlpha(.7);
	CEGUI::Imageset* imageSet = CEGUI::ImagesetManager::getSingleton().getImageset((CEGUI::utf8*)"CameraBut");
	cameraButton->setProperty("NormalImage", CEGUI::PropertyHelper::imageToString(
		&imageSet->getImage((CEGUI::utf8*)"cameraNormal")));
	cameraButton->setProperty("PushedImage", CEGUI::PropertyHelper::imageToString(
		&imageSet->getImage((CEGUI::utf8*)"cameraPushed")));
	cameraButton->setProperty("HoverImage", CEGUI::PropertyHelper::imageToString(
		&imageSet->getImage((CEGUI::utf8*)"cameraHover")));
	window->addChildWindow(cameraButton);
	cameraButton->setSize(CEGUI::UVector2( CEGUI::UDim(0.1f, 0), CEGUI::UDim(0.13f, 0)));
	cameraButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.128125f, 0), CEGUI::UDim(0.02125f, 0)));
	cameraButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleScreenshot, this));

	CEGUI::Window* trashButton =CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/ImageButton", (CEGUI::utf8*)"Trash");
	trashButton->setAlpha(.7);
	CEGUI::Imageset* imageSet2 = CEGUI::ImagesetManager::getSingleton().getImageset((CEGUI::utf8*)"TrashCan");
	trashButton->setProperty("NormalImage", CEGUI::PropertyHelper::imageToString(
		&imageSet2->getImage((CEGUI::utf8*)"TrashCanNormal")));
	trashButton->setProperty("PushedImage", CEGUI::PropertyHelper::imageToString(
		&imageSet2->getImage((CEGUI::utf8*)"TrashCanPushed")));
	trashButton->setProperty("HoverImage", CEGUI::PropertyHelper::imageToString(
		&imageSet2->getImage((CEGUI::utf8*)"TrashCanHover")));
	window->addChildWindow(trashButton);
	trashButton->setSize(CEGUI::UVector2( CEGUI::UDim(0.1f, 0), CEGUI::UDim(0.13f, 0)));
	trashButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.864062f, 0), CEGUI::UDim(0.021250f, 0)));
	trashButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleTrash, this));

}

void PresenceApplication::loadMenuItems(int numberOfObjects){
	CEGUI::Window* editorWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Menu");

	CEGUI::Window* window = 0;

	Real posX = 0.05; //Math::RangeRandom(0.0, 0.0); 
	Real posY = 0.05; //Math::RangeRandom(0.0, 0.0);

	for(int i=0; i<numberOfObjects; i++){
		window = createStaticImageObject();
		editorWindow->addChildWindow(window);
		window->setPosition(CEGUI::UVector2(CEGUI::UDim(posX, 0), CEGUI::UDim(posY, 0)));
		if(posX > .8 && posY <.8) {
			posX = .05;
			posY += .12;
		}
		else posX += 0.1;

	}
}

CEGUI::Window* PresenceApplication::createStaticImageObject(void)
{
	static unsigned int siCounter = 0;
	String guiObjectName = "Objects" + StringConverter::toString(siCounter);

	CEGUI::Imageset* imageSet = 
		CEGUI::ImagesetManager::getSingleton().getImageset(
		(CEGUI::utf8*)"ObjectBut");

	CEGUI::Window* si = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/ImageButton",
		(CEGUI::utf8*)guiObjectName.c_str());
	si->setSize(CEGUI::UVector2( CEGUI::UDim(0.1f, 0), CEGUI::UDim(0.13f, 0)));
	si->setProperty("NormalImage", CEGUI::PropertyHelper::imageToString(
		&imageSet->getImage((CEGUI::utf8*)"treeDefault")));
	si->setProperty("HoverImage", CEGUI::PropertyHelper::imageToString(
		&imageSet->getImage((CEGUI::utf8*)"treeHover")));
	si->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleMenuObjects, this));

	siCounter++;

	return si;
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
