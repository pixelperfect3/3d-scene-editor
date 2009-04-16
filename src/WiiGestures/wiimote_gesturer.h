#pragma once

#include "math.h"
#define PI 3.14159265898

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

double Dot(double b[2], double a[3], double c[3]) {
	double v1[2] = { b[0] - a[0], b[1] - a[1] };
	double v2[2] = { c[0] - a[0], c[1] - a[1] };
	//double mag1 = sqrt(v1[0] * v1[0] + v1[1] * v1[1]);
	//double mag2 = sqrt(v2[0] * v2[0] + v2[1] * v2[1]);
	//v1[0] /= mag1;
	//v1[1] /= mag1;
	//v2[0] /= mag2;
	//v2[1] /= mag2;
	return v1[0] * v2[0] + v1[1] + v2[1];
}

class WiiMoteGesturer {
protected:
	WiiMoteClient *wiimote;
	int previous_points;
	double midpoint[2];
	double old_mouse[3];
	double prev_center[2];
	double prev_tangent[2];
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
		//std::cout << "Size: " << wiimote->ir_size[0];
		//std::cout << "Active points: " << active_points << "\n";
		bool newGesture = active_points != previous_points;
		if (newGesture) {
			ToggleMouseCursor(active_points == 1);
		}
		switch (active_points) {
			case 1:
				{
					if (!newGesture) {
						double distance[2];
						Distance(points[0], old_mouse, distance); //TODO : necessary?
						MouseMoved(points[0], old_mouse, distance);
						for (int ii = 0; ii < 3; ii++) {
							old_mouse[ii] = points[0][ii];
						}
					}
				}
				break;
			case 3:
				double distance[2];
				double tmp[2];
				Midpoint(points[0], points[1], tmp);
				Distance(tmp, midpoint, distance);
				if (!newGesture) {
					Translate(distance);
				}
				for (int ii = 0; ii < 2; ii++) {
					midpoint[ii] = tmp[ii];
				}
				break;
			case 2:
				{
					int center = 0;
					int tangent = 1;
					double tmp[2];
					Distance(points[center], points[tangent], tmp);
					if (!newGesture) {
						double dot = Dot(prev_tangent, points[center], points[tangent]);
						double delta = acos(dot) * 180.0 / PI;
						std::cout << "Delta angle: " << delta << " (dot=" << dot << ")\n";
						Rotate(delta);
					}
					for (int ii = 0; ii < 2; ii++) {
						prev_center[ii] = points[center][ii];
						prev_tangent[ii] = points[tangent][ii];
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