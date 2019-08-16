#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

class Keyboard{
public:
	Keyboard();
	void OnKeyPress(WPARAM wParam, LPARAM lParam);
	void OnKeyRelease(WPARAM wParam, LPARAM lParam);
	void OnChar();
};