#pragma once

#include <ogre.h>
#include <CEGUI/CEGUI.h>

class ScreenShotFrameListener : public FrameListener {
private:
	bool screenShotNextFrame;
	bool screenShotThisFrame;
	bool screenShotLastFrame;
	bool restoreState[5];

protected:
	ExampleFrameListener *delegateListener;
public:
	ScreenShotFrameListener(ExampleFrameListener *delegateListener) :
	screenShotNextFrame(false), screenShotThisFrame(false), screenShotLastFrame(false) {
		screenShotLastFrame = false;
		screenShotThisFrame = false;
		screenShotLastFrame = false;
		this->delegateListener = delegateListener;
	}
	~ScreenShotFrameListener() {
		delegateListener = NULL;
	}
	
	
	bool frameStarted(const FrameEvent &evt) {
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		if (screenShotNextFrame) {
			restoreState[0] = wmgr.getWindow("Models")->isVisible();
			restoreState[1] = wmgr.getWindow("CameraButton")->isVisible();
			restoreState[2] = wmgr.getWindow("Trash")->isVisible();
			restoreState[3] = wmgr.getWindow("Menu")->isVisible();
			restoreState[4] = CEGUI::MouseCursor::getSingleton().isVisible();
			screenShotThisFrame = true;
			screenShotNextFrame = false;
			wmgr.getWindow("Models")->setVisible(false);
			wmgr.getWindow("CameraButton")->setVisible(false);
			wmgr.getWindow("Trash")->setVisible(false);
			wmgr.getWindow("Menu")->setVisible(false);
			CEGUI::MouseCursor::getSingleton().setVisible(false);
		} else if (screenShotThisFrame) {
			delegateListener->screenCapture();
			screenShotThisFrame = false;
			screenShotLastFrame = true;
		} else if (screenShotLastFrame) {
			wmgr.getWindow("Models")->setVisible(restoreState[0]);
			wmgr.getWindow("CameraButton")->setVisible(restoreState[1]);
			wmgr.getWindow("Trash")->setVisible(restoreState[2]);
			wmgr.getWindow("Menu")->setVisible(restoreState[3]);
			CEGUI::MouseCursor::getSingleton().setVisible(restoreState[4]);
			screenShotLastFrame = false;
		}
		return true;
	}

	bool handleScreenshot(const CEGUI::EventArgs& e) {
		if (!screenShotNextFrame) {
			screenShotNextFrame = true; //TODO : race condition?
		}
		return true;
	}
};