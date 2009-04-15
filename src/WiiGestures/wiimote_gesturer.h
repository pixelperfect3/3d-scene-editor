#pragma once

#include "math.h"

void ToggleMouseCursor(bool useMouse); // (toggles mouse cursor visibility).
void DrawPoints(int num_points, double points[4][3]);
void MouseMoved(double mouse[3], double old_mouse[3], double distance[2]);
void Translate(double distance[2]);
void Rotate(double degrees);

double UnitizeX(double x) {
	return 2.0 * x / (double) VRPN_WIIMOTE_MAX_IR_X - 1.0;
}
double UnitizeY(double y) {
	return 2.0 * y / (double) VRPN_WIIMOTE_MAX_IR_Y - 1.0;
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

class WiiMoteGesturer {
protected:
	WiiMoteClient *wiimote;
	int previous_points;
	double midpoint[2];
	double old_mouse[3];
public:
	WiiMoteGesturer(WiiMoteClient *wiimote) : previous_points(0) {
		this->wiimote = wiimote;
	}

/* Each frame:
 * + get available/active points.
 * + if the number of points changes:
 *   + determine which gesture is active.
 *   + fire the appropriate gesture-changed event.
 * + otherwise, determine what changes (model/selection/...) are necessary for the active gesture.
 *
 * Additional (unnecessary?) error-checking:
 *   When there is a change, wait a specified number of frames for occlusion or tracking errors to be fixed.
 *   Even when number of points don't change,
 *     compare current positions to previous positions (in case points get swapped?).
 *   When number of points changes, foreach previously active point:
 *     compare position of active points to its position and find "closest" match, if any.
 *     if closest match is within a specific distance, the point is still active.
 *     otherwise, the point is inactive.
 */
	void frameStarted(double deltaTime) { //once per frame
		double points[4][3] = { {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1} };

		int active_points = 0;
		for (int ii = 0; ii < 4; ii++) {
			if (wiimote->ir_size[ii] > 0) {
				points[active_points][0] = UnitizeX(wiimote->ir_x[ii]);
				points[active_points][1] = UnitizeY(wiimote->ir_y[ii]);
				points[active_points][2] = wiimote->ir_size[ii];
				active_points++;
			}
		}
		if (active_points != previous_points) {
			ToggleMouseCursor(active_points == 1);
		}
		switch (active_points) {
			case 1:
				{
					if (previous_points == 1) {
						double distance[2];
						Distance(points[0], old_mouse, distance); //TODO : necessary?
						MouseMoved(points[0], old_mouse, distance);
						for (int ii = 0; ii < 3; ii++) {
							old_mouse[ii] = points[0][ii];
						}
					}
				}
				break;
			case 2:
				double distance[2];
				double tmp[2];
				Midpoint(points[0], points[1], tmp);
				Distance(tmp, midpoint, distance);
				for (int ii = 0; ii < 2; ii++) {
					midpoint[ii] = tmp[ii];
				}
				if (previous_points == 2) {
					Translate(distance);
				}
				break;
			case 3:
				{	//TODO : stub
					double degrees = 0;
					if (previous_points == 3) {
						Rotate(degrees);
					}
				}
				break;
			case 4:
				//TODO : stub
				break;
		}
		DrawPoints(active_points, points);
		previous_points = active_points;
	}
};