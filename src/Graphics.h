#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include "d3dcompiler.h"
#include <DirectXMath.h>
#include <wrl.h>
#include <iostream>

// Macro for handling HRESULT graphics errors from Direct3D API calls.
#define GFX_ERROR_CHECK(hr)

class Graphics{
public:
	Graphics(HWND hWnd, int WIDTH, int HEIGHT, int REFRESH_RATE);
	~Graphics() {
		/*this->pVertexShader->Release();
		this->pPixelShader->Release();
		this->pRenderTargetView->Release();
		this->pDeviceContext->Release();
		this->pSwapChain->Release();
		this->pDevice->Release();*/
	}

	void Clear(float=0.0f, float=0.0f, float=0.0f, float=1.0f);

	void BeginFrame();
	void Draw();
	void EndFrame();
private:
	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSView;
};