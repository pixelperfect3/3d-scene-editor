#include "PresenceApplication.h"
#include "KeyboardGestureDriver.h"
#include "ogreconsole.h"

PresenceApplication::PresenceApplication(char* ir_tracker_name, char* nunchuk_name) {
	if (ir_tracker_name) {
		wiimote = new WiiMoteClient(ir_tracker_name);
		wiimote->set_mode(VRPN_WIIMOTE_CHANNEL_MODE_IR, true);
		std::cout << "Using Wiimote \"" << ir_tracker_name << "\" (for IR tracking).\n";
	} else {
		wiimote = NULL;
		std::cout << "Not using Wiimote for IR tracking.\n";
	}
	if (nunchuk_name) {
		nunchuk = new WiiMoteClient(nunchuk_name);
		std::cout << "Using Wiimotes \"" << nunchuk_name << "\" (for nunchuk).\n";
	} else {
		nunchuk = NULL;
		std::cout << "Not using Wiimote for nunchuk inputs.\n";
	}
	mGUIRenderer =0;
	mGUISystem = 0;
	mEditorGuiSheet = 0;

	char* temp[NUM_OBJMODELS] = {
		"tree_amarelo.mesh",
		"tree_magnolia.mesh",
		"tree_bamboo.mesh",
		"tree_cabbagepalm.mesh",
		"tree_palm.mesh",
		"plant_sagopalm.mesh",
		"plant_yucca.mesh",
		"plant_red.mesh",
		"plant_octopus.mesh",
		"plant_monstera.mesh",
		"chair_wood.mesh",
		"chair_set_rustic.mesh",
		"chair_lounge.mesh",
		"chair_cushion.mesh",
		"chair_set_plastic.mesh"
	};
	for(int i=0;i<NUM_OBJMODELS;i++)
		buttonobjModels[i] = temp[i];
	//Fixes errors in destructor:
	model_manager = NULL;
	fsm = NULL;
}

PresenceApplication::~PresenceApplication() {
	if (nunchuk && nunchuk != wiimote) {
		delete nunchuk;
	}
	if (wiimote) {
		mRoot->removeFrameListener(gesturer);
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
	if (fsm) {
		delete fsm;
	}
	if (model_manager) {
		delete model_manager;
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

// Sets up all the audio
void PresenceApplication::createAudio(void) 
{
	// Create an audio manager
	audMgr = new AudioManager();

	// Add the audio files
	// source pos
	ALfloat SourcePos[3] = { 0,0,0 };

	// Velocity of the source sound.
	ALfloat SourceVel[] = { 0.0f, 0.0f, 0.0f };
	audMgr->addSound("camera", "res\\audio\\camera.wav", SourcePos, SourceVel, 0);
	audMgr->addSound("click", "res\\audio\\click1.wav", SourcePos, SourceVel, 0);
	audMgr->addSound("trash", "res\\audio\\recycle.wav", SourcePos, SourceVel, 0);
	//audMgr->addSound("track1", "res\\audio\\track1.wav", SourcePos, SourceVel, 0);

}

// Just override the mandatory create scene method
void PresenceApplication::createScene(void)
{

	new OgreConsole;
	OgreConsole::getSingleton().init(mRoot);
	OgreConsole::getSingleton().setVisible(false);

	mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
	//if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_HWRENDER_TO_TEXTURE))
 //   {
 //       // In D3D, use a 1024x1024 shadow texture
 //       mSceneMgr->setShadowTextureSettings(1024, 2);
 //   }
 //   else
 //   {
 //       // Use 512x512 texture in GL since we can't go higher than the window res
 //       mSceneMgr->setShadowTextureSettings(512, 2);
 //   }
    mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowFarDistance(10000);

	mSceneMgr->setAmbientLight(ColourValue(0.5,0.5,0.5,1));
	//mSceneMgr->setAmbientLight(ColourValue(0,0,0));

	Light* light = mSceneMgr->createLight("mainLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDiffuseColour(0.5,0.5,0.5);
	light->setSpecularColour(0.5,0.5,0.5);
	light->setDirection(1,-1,1	);
	//light->setCastShadows(true);

	mSceneMgr->setSkyBox(true, "Examples/Skybox");
	
	
	mainSceneNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild("mainScene"));
	Entity* mainSceneEn = mSceneMgr->createEntity("home","home.mesh");
	mainSceneNode->attachObject(mainSceneEn);
	mainSceneEn->setCastShadows(false);
	
	SceneNode *lawnNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild("lawn"));
	Entity* lawnEn = mSceneMgr->createEntity("lawn","lawn.mesh");
	lawnNode->attachObject(lawnEn);

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
	loadMenuItems(NUM_OBJMODELS);

	setupEnterExitEvents(wmgr.getWindow("Menu"));

	mTip = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/StaticText", (CEGUI::utf8*)"ToolTips");
	mTip->setSize(CEGUI::UVector2(CEGUI::UDim(0.65f,0), CEGUI::UDim(0.08f,0)));
	mTip->setPosition(CEGUI::UVector2(CEGUI::UDim(.08125f, 0), CEGUI::UDim(.9000f, 0)));
	wmgr.getWindow("Menu")->addChildWindow(mTip);

	mDescriptionMap[(CEGUI::utf8*)"Cancel"] = 
		(CEGUI::utf8*)" Close Model Selector";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton1"] = 
		(CEGUI::utf8*)" Amarillo Tree";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton2"] = 
		(CEGUI::utf8*)" Bamboo Tree";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton3"] = 
		(CEGUI::utf8*)" Cabbage Palm Tree";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton4"] = 
		(CEGUI::utf8*)" Magnolia Tree";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton5"] = 
		(CEGUI::utf8*)" Palm Tree";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton6"] = 
		(CEGUI::utf8*)" Sago Palm Tree";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton7"] = 
		(CEGUI::utf8*)" Yucca Plant";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton8"] = 
		(CEGUI::utf8*)" Red Plant";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton9"] = 
		(CEGUI::utf8*)" Octopus Plant";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton10"] = 
		(CEGUI::utf8*)" Monstera Plant";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton11"] = 
		(CEGUI::utf8*)" Wood Chair";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton12"] = 
		(CEGUI::utf8*)" Rusti Chair Set";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton13"] = 
		(CEGUI::utf8*)" Lounge Chair";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton14"] = 
		(CEGUI::utf8*)" Cushion Chair";
	mDescriptionMap[(CEGUI::utf8*)"MenuButton15"] = 
		(CEGUI::utf8*)" Plastic Chair Set";


	init_pointers(mSceneMgr);
}

void PresenceApplication::setupEventHandlers(){
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* window = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Root");
	

	wmgr.getWindow("Menu")->setVisible(false); // menu is invisible
	//wmgr.getWindow("Menu")->setAlpha(.8);
	
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
	cameraButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.198125f, 0), CEGUI::UDim(0.02125f, 0)));
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

	CEGUI::Window* undoButton =CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/ImageButton", (CEGUI::utf8*)"Undo");
	undoButton->setAlpha(.7);
	CEGUI::Imageset* imageSet3 = CEGUI::ImagesetManager::getSingleton().getImageset((CEGUI::utf8*)"UndoSet");
	undoButton->setProperty("NormalImage", CEGUI::PropertyHelper::imageToString(
		&imageSet3->getImage((CEGUI::utf8*)"UndoNormal")));
	undoButton->setProperty("PushedImage", CEGUI::PropertyHelper::imageToString(
		&imageSet3->getImage((CEGUI::utf8*)"UndoPushed")));
	undoButton->setProperty("HoverImage", CEGUI::PropertyHelper::imageToString(
		&imageSet3->getImage((CEGUI::utf8*)"UndoHover")));
	window->addChildWindow(undoButton);
	undoButton->setSize(CEGUI::UVector2( CEGUI::UDim(0.1f, 0), CEGUI::UDim(0.13f, 0)));
	undoButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.71062f, 0), CEGUI::UDim(0.021250f, 0)));
	undoButton->subscribeEvent(
		CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleUndo, this));

}

void PresenceApplication::loadMenuItems(int numberOfObjects){
	CEGUI::Window* editorWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Menu");

	CEGUI::Window* window = 0;

	Real posX = 0.05; //Math::RangeRandom(0.0, 0.0);
	Real posY = 0.05; //Math::RangeRandom(0.0, 0.0);
	unsigned int buttonCounter = 1;
	unsigned int objectCounter = 1;

	CEGUI::Imageset* imageSet = 
		CEGUI::ImagesetManager::getSingleton().getImageset(
		(CEGUI::utf8*)"MenuObjects");

	for(int i=0; i<numberOfObjects; i++){
		String guiObjectName = "MenuButton" + StringConverter::toString(buttonCounter++);
		String MenuObjectNorm = "NewImage" + StringConverter::toString(objectCounter++);
		String MenuObjectHover = "NewImage" + StringConverter::toString(objectCounter++);

		CEGUI::Window* button = CEGUI::WindowManager::getSingleton().createWindow((CEGUI::utf8*)"TaharezLook/ImageButton",
			(CEGUI::utf8*)guiObjectName.c_str());
		button->setSize(CEGUI::UVector2( CEGUI::UDim(0.1f, 0), CEGUI::UDim(0.13f, 0)));
		button->setProperty("NormalImage", CEGUI::PropertyHelper::imageToString(
			&imageSet->getImage((CEGUI::utf8*)MenuObjectNorm.c_str())));
		
		button->setProperty("HoverImage", CEGUI::PropertyHelper::imageToString(
			&imageSet->getImage((CEGUI::utf8*)MenuObjectHover.c_str())));
		button->subscribeEvent(
			CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PresenceApplication::handleMenuObjects, this));

		editorWindow->addChildWindow(button);
		button->setPosition(CEGUI::UVector2(CEGUI::UDim(posX, 0), CEGUI::UDim(posY, 0)));
		if(posX > .8 && posY <.8) {
			posX = .05;
			posY += .14;
		}
		else posX += 0.1;
	}
}


void PresenceApplication::createFrameListener(void) {
	model_manager = new ModelManager(mSceneMgr, mCamera);
	fsm = new GestureFSM(model_manager);
	KeyboardGestureDriver *driver = new KeyboardGestureDriver(fsm, model_manager, mWindow, mCamera, mGUIRenderer, nunchuk, audMgr);
	mFrameListener = driver;
    mRoot->addFrameListener(mFrameListener);
	if (wiimote) {
		gesturer = new WiiMoteGesturer(wiimote, driver, model_manager);
		mRoot->addFrameListener(gesturer);
	} else {
		gesturer = NULL;
	}
}
