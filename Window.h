#pragma once
#include "Main.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window{
public:
	Window(HINSTANCE hInstance, LPCWSTR title, int width, int height);
	void SetTitle(LPCWSTR title);
	HWND GetHandler();

	void ProcessMessagesLoop();
	bool ProcessMessages();

private:
	// Windows event handling.
	static LRESULT CALLBACK WndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcForward(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND hWnd;
	Keyboard keyb;
	Mouse mouse;
};