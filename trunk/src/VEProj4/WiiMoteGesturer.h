#pragma once

#include "ogre.h"
#include "wiimote_client.h"
#include "model_manager.h"
#include "mouseDriver.h"
#include "Pointers.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265898
#endif

//Defined elsewhere (later):
static void DrawPoints(int num_points, double points[4][3]); //Pointers.h

class WiiMoteGesturer : public FrameListener {
private:
	int previous_points;
	double midpoint[2];
	double old_mouse[3];
	Radian prev_angle;
	double points[4][3];
protected:
	WiiMoteClient *wiimote;
	MouseDriver *mouseDriver;
	ModelManager *modelManager;
public:
	WiiMoteGesturer(WiiMoteClient *wiimote, MouseDriver *mouseDriver, ModelManager *manager) :
	previous_points(0), prev_angle(0) {
		assert(wiimote);
		this->wiimote = wiimote;
		this->mouseDriver = mouseDriver;
		this->modelManager = manager;
	}
	virtual ~WiiMoteGesturer() {
		this->wiimote = NULL;
		this->mouseDriver = NULL;
		this->modelManager = NULL;
	}

	virtual bool frameStarted(const FrameEvent &evt) {
		wiimote->updateFromServer(); //may incur frame lag.
		for (int ii = 0; ii < 4; ii++) {
			points[ii][0] = points[ii][1] = 0;
			points[ii][2] = -1;
		}

		int active_points = 0;
		for (int ii = 0; ii < 4; ii++) {
			if (wiimote->ir_size[ii] > 0) {
				points[active_points][0] = UnitizeX(wiimote->ir_x[ii]);
				points[active_points][1] = UnitizeY(wiimote->ir_y[ii]);
				points[active_points][2] = wiimote->ir_size[ii];
				active_points++;
			}
		}
		//std::cout << "Size: " << wiimote->ir_size[0];
		//std::cout << "Active points: " << active_points << "\n";
		bool newGesture = active_points != previous_points;
		if (newGesture) {
			mouseDriver->ToggleMouseCursor(active_points <= 1);
		}
		DrawPoints(active_points, points);
		previous_points = active_points;
		doMouse(newGesture);
		if (active_points > 1) {
			doTranslation(newGesture);
			doRotation(newGesture);
		}
		return true;
	}
protected:	
	void doMouse(bool newGesture) {
		if (!newGesture) {
			double distance[2];
			Distance(points[0], old_mouse, distance);
			mouseDriver->MouseMoved(points[0], old_mouse, distance);
			for (int ii = 0; ii < 3; ii++) {
				old_mouse[ii] = points[0][ii];
			}
		}
	}
	void doTranslation(bool newGesture) {
		double distance[2];
		double tmp[2];
		Midpoint(points[0], points[1], tmp);
		Distance(tmp, midpoint, distance);
		if (!newGesture) {
			modelManager->Translate2D(distance, Vector2(midpoint[0], midpoint[1]));
		}
		for (int ii = 0; ii < 2; ii++) {
			midpoint[ii] = tmp[ii];
		}
	}
	void doRotation(bool newGesture) {
		bool leftToRight = points[0][0] < points[1][0];
		int leftIndex = leftToRight ? 0 : 1;
		int rightIndex = leftToRight ? 1 : 0;
		Vector2 left = Vector2(points[leftIndex][0], points[leftIndex][1]);
		Vector2 right = Vector2(points[rightIndex][0], points[rightIndex][1]);
		if (left.x == right.x) {
			std::cout << "Arc-tan not defined.\n";
			prev_angle = Radian(90);
			return;
		}
		Radian angle = Radian(atan((right.y - left.y) / (right.x - left.x)));
		if (!newGesture) {
			modelManager->rotate_update(prev_angle - angle);
		}
		prev_angle = angle;
	}
	double Unitize(double val, double maxUnits, double percentFOV) {
		double new_val = val / maxUnits;
		new_val = max(percentFOV, new_val);
		new_val = min(1.0 - percentFOV, new_val);
		new_val = (new_val - percentFOV) / (1.0 - 2.0 * percentFOV);
		return new_val;
	}
	double UnitizeX(double x) {
		return Unitize(x, (double) VRPN_WIIMOTE_MAX_IR_X, 0.25);
	}
	double UnitizeY(double y) {
		return Unitize(y, (double) VRPN_WIIMOTE_MAX_IR_Y, 0.25);
	}

	double Distance(double a[3], double b[3]) {
		double deltaX = b[0] - a[0];
		double deltaY = b[1] - a[1];
		return sqrt(deltaX * deltaX + deltaY * deltaY);
	}
	void Distance(double a[3], double b[3], double dest[2]) {
		for (int ii = 0; ii < 2; ii++) {
			dest[ii] = b[ii] - a[ii];
		}
	}
	void Midpoint(double a[3], double b[3], double dest[2]) {
		for (int ii = 0; ii < 2; ii++) {
			dest[ii] = (b[ii] + a[ii]) / 2;
		}
	}
};
