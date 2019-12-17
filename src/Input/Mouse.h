#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

#define MOUSE_BUTTON_COUNT 3
const enum MOUSE_BUTTON {
	LEFT_BUTTON,
	RIGHT_BUTTON,
	MIDDLE_BUTTON
};

class Mouse {
public:
	Mouse();

	// Game Engine
	void Reset();

	bool isButtonPressed(MOUSE_BUTTON button);
	bool isButtonHeld(MOUSE_BUTTON button);
	bool isButtonReleased(MOUSE_BUTTON button);

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

	// Button states of mouse device
	bool buttonStates[MOUSE_BUTTON_COUNT];
	bool buttonPreviousStates[MOUSE_BUTTON_COUNT];

	unsigned int posX = 0;
	unsigned int posY = 0;
	signed long rawAccumulateX = 0;
	signed long rawAccumulateY = 0;

	signed short wheelAccumulate = 0;
	int wheelRotateCountDirection;

	LPRECT pRect;
private:
	bool confined = false;
};