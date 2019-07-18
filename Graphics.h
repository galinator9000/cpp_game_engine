#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>

class Graphics{
public:
	Graphics(HWND hWnd, int WIDTH, int HEIGHT, int REFRESH_RATE);
	void Clear(float r, float g, float b, float a);
	void Clear();
	void BeginFrame();
	void EndFrame();
private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
};