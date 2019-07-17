#include <Windows.h>
#include "Window.h"

Window::Window(HINSTANCE hInstance, LPCWSTR title, int width, int height) {
	WNDCLASSEX wc = { 0 };
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProcSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.cbSize = sizeof(wc);
	RegisterClassEx(&wc);

	hWnd = CreateWindow(
		title, title,
		(WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU),
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL, NULL, hInstance, this
	);
	ShowWindow(hWnd, SW_SHOW);
}

LRESULT Window::WndProcSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_NCCREATE){
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WndProcForward));
		return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::WndProcForward(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	// Keyboard messages
	case WM_KEYDOWN:
		this->keyb.OnKeyPress();
		break;

	case WM_KEYUP:
		this->keyb.OnKeyRelease();
		break;

	case WM_CHAR:
		this->keyb.OnChar();
		break;

	// Mouse messages
	case WM_LBUTTONDOWN:
		this->mouse.OnLeftPress(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		this->mouse.OnLeftRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDOWN:
		this->mouse.OnRightPress(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONUP:
		this->mouse.OnRightRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEMOVE:
		this->mouse.OnMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL:
		this->mouse.OnWheelMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case WM_MOUSELEAVE:
		this->mouse.OnLeave();
		break;
	case WM_MOUSEHOVER:
		this->mouse.OnHover();
		break;

	case WM_CLOSE:
		PostQuitMessage(USER_EXIT);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
}

bool Window::ProcessMessages(){
	// Process all messages on queue.
	MSG Msg;
	while(PeekMessage(&Msg, this->hWnd, 0, 0, PM_REMOVE) != 0){
		if (Msg.message == WM_QUIT) {
			return false;
		}

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	};
	return true;
}

void Window::SetTitle(LPCWSTR title){
	SetWindowText(hWnd, title);
}

HWND Window::GetHandler(){
	return this->hWnd;
}
