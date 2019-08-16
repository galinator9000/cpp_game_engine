#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

class Mouse{
public:
	Mouse();
	void OnLeftPress(WPARAM wParam, LPARAM lParam);
	void OnLeftRelease(WPARAM wParam, LPARAM lParam);
	void OnRightPress(WPARAM wParam, LPARAM lParam);
	void OnRightRelease(WPARAM wParam, LPARAM lParam);
	void OnMove(WPARAM wParam, LPARAM lParam);
	void OnWheelMove(WPARAM wParam, LPARAM lParam);
	void OnLeave(WPARAM wParam, LPARAM lParam);
	void OnHover(WPARAM wParam, LPARAM lParam);

	bool leftPressed;
	bool rightPressed;

	int GetX();
	int GetY();

private:
	int x, y;
	int wheelAccumulate;
};