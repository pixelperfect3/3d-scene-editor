#pragma once

class MouseDriver {
public:
	virtual void ToggleMouseCursor(bool useMouse) = 0;
	virtual void MouseMoved(double mouse[3], double old_mouse[3], double distance[2]) = 0;
};