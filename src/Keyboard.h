#pragma once

#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>
#include <bitset>

class Keyboard{
public:
	Keyboard();

	void Reset();

	// Game Engine
	bool isKeyPressed(unsigned int key);

	// Windows procedure
	void OnKeyPress(WPARAM wParam, LPARAM lParam);
	void OnKeyRelease(WPARAM wParam, LPARAM lParam);
	void OnChar(WPARAM wParam, LPARAM lParam);

private:
	std::bitset<256> keyStatus;
};