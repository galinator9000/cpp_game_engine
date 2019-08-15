#pragma once
#include "Camera.h"
#include "Entity/BaseEntity.h"
#include "Entity/Structs.h"
#include "Entity/Shapes.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include "d3dcompiler.h"
#include <DirectXMath.h>
#include <wrl.h>
#include <iostream>
#include <vector>

// Macro for handling HRESULT graphics errors from Direct3D API calls.
#define GFX_ERROR_CHECK(hr)

class Graphics{
public:
	Graphics(HWND hWnd, unsigned int WIDTH, unsigned int HEIGHT, int REFRESH_RATE);
	~Graphics() {
		/*this->pVertexShader->Release();
		this->pPixelShader->Release();
		this->pRenderTargetView->Release();
		this->pDeviceContext->Release();
		this->pSwapChain->Release();
		this->pDevice->Release();*/
	}

	void Clear(float=0.0f, float=0.0f, float=0.0f, float=1.0f);

	void addEntity(BaseEntity* entity);
	void beginFrame();
	void drawEntity(BaseEntity* entity);
	void endFrame();
	
	void addCamera(Camera* camera, bool setAsMain);
	void setCamera(Camera* camera);
	void updateCamera(Camera* camera);

	unsigned int gWidth;
	unsigned int gHeight;
private:
	HRESULT hr;

	// Device side
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;

	// Pipeline
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSView;
};