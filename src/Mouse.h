#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

class Mouse {
public:
	Mouse();

	// Game Engine
	void resetRawAccumulate();
	bool confineCursor();
	bool freeCursor();
	void hideCursor();
	void showCursor();

	// Windows procedure
	void OnLeftPress(WPARAM wParam, LPARAM lParam);
	void OnLeftRelease(WPARAM wParam, LPARAM lParam);
	void OnRightPress(WPARAM wParam, LPARAM lParam);
	void OnRightRelease(WPARAM wParam, LPARAM lParam);
	void OnMove(WPARAM wParam, LPARAM lParam);
	void OnMoveRaw(signed long deltaX, signed long deltaY);
	void OnWheelMove(WPARAM wParam, LPARAM lParam);
	void OnLeave(WPARAM wParam, LPARAM lParam);
	void OnHover(WPARAM wParam, LPARAM lParam);

	bool leftPressed = false;
	bool rightPressed = false;

	unsigned int posX = 0;
	unsigned int posY = 0;
	signed long rawAccumulateX = 0;
	signed long rawAccumulateY = 0;

	LPRECT pRect;
private:
	bool confined = false;
};