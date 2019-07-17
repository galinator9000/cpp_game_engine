#pragma once

class Mouse{
public:
	Mouse();
	void OnLeftPress(int xx, int yy);
	void OnLeftRelease(int xx, int yy);
	void OnRightPress(int xx, int yy);
	void OnRightRelease(int xx, int yy);
	void OnMove(int xx, int yy);
	void OnWheelMove(int xx, int yy, int wheelDelta);
	void OnLeave();
	void OnHover();

	bool leftPressed;
	bool rightPressed;

private:
	int x, y;
	int wheelAccumulate;
};