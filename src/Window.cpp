#include <Windows.h>
#include "Window.h"

Window::Window(HINSTANCE hInstance, LPCWSTR title, int width, int height) {
	this->hInstance = hInstance;
	this->title = title;
	this->width = width;
	this->height = height;

	WNDCLASSEX wc = { 0 };
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProcSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->title;
	wc.cbSize = sizeof(wc);
	RegisterClassEx(&wc);

	// Create window from API
	this->hWnd = CreateWindow(
		this->title, this->title,
		(WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU),
		CW_USEDEFAULT, CW_USEDEFAULT, this->width, this->height,
		NULL, NULL, this->hInstance, this
	);
	ShowWindow(this->hWnd, SW_SHOW);

	// Register raw mouse input
	RAWINPUTDEVICE rid = {0};
	rid.usUsagePage = 1;
	rid.usUsage = 2;
	rid.dwFlags = 0;
	rid.hwndTarget = NULL;
	RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void Window::Setup(Keyboard* keyboard, Mouse* mouse) {
	this->keyb = keyboard;
	this->mouse = mouse;
	this->updateBounds();
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
	// Key strokes
	case WM_KEYDOWN:
		this->keyb->OnKeyPress(wParam, lParam);
		break;
	case WM_KEYUP:
		this->keyb->OnKeyRelease(wParam, lParam);
		break;

	// Character
	case WM_CHAR:
		this->keyb->OnChar(wParam, lParam);
		break;
	case WM_UNICHAR:
		this->keyb->OnChar(wParam, lParam);
		break;

	// Mouse messages
	case WM_MOUSEMOVE:
		this->mouse->OnMove(wParam, lParam);
		break;
	case WM_MOUSELEAVE:
		this->mouse->OnLeave(wParam, lParam);
		break;
	case WM_MOUSEHOVER:
		this->mouse->OnHover(wParam, lParam);
		break;
	case WM_LBUTTONDOWN: {
		this->updateBounds();

		// Confine cursor to current window's bounds.
		this->mouse->confineCursor();
		this->mouse->OnLeftPress(wParam, lParam);
		break;
	}
	case WM_LBUTTONUP:
		this->mouse->OnLeftRelease(wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
		this->mouse->OnRightPress(wParam, lParam);
		break;
	case WM_RBUTTONUP:
		this->mouse->OnRightRelease(wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		this->mouse->OnWheelMove(wParam, lParam);
		break;

	// Raw messages
	case WM_INPUT: {
		unsigned int pcbSize = 0;
		GetRawInputData(
			(HRAWINPUT) lParam,
			RID_INPUT,
			NULL,
			&pcbSize,
			sizeof(RAWINPUTHEADER)
		);

		byte* pData = new byte[pcbSize];
		GetRawInputData(
			(HRAWINPUT) lParam,
			RID_INPUT,
			pData,
			&pcbSize,
			sizeof(RAWINPUTHEADER)
		);
		RAWINPUT* raw = (RAWINPUT*) pData;

		// Raw mouse handling
		if (raw->header.dwType == RIM_TYPEMOUSE) {
			this->mouse->OnMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
		}

		break;
	};

	// Other messages
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
}

void Window::updateBounds() {
	// Update window bound values.
	RECT rect = RECT({ 0 });
	if (GetWindowRect(this->hWnd, &rect)) {
		this->mouse->pRect->left = rect.left;
		this->mouse->pRect->top = rect.top;
		this->mouse->pRect->right = rect.right;
		this->mouse->pRect->bottom = rect.bottom;
	}
}

bool Window::ProcessMessages(){
	// Process messages on queue.
	MSG Msg;
	while (PeekMessage(&Msg, this->hWnd, 0, 0, PM_REMOVE) != 0) {
		if (Msg.message == WM_QUIT) {
			return false;
		}

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	};
	return true;
}

void Window::SetTitle(LPCWSTR title){
	SetWindowText(this->hWnd, title);
}

HWND Window::GetHandler(){
	return this->hWnd;
}
