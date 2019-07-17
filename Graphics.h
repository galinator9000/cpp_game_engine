#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>

class Graphics{
public:
	Graphics(HWND hWnd, int WIDTH, int HEIGHT, int REFRESH_RATE);
	void UpdateFrame();
private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
};