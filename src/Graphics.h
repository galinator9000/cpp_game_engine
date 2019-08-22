#pragma once
#include "Config.h"
#include "Camera.h"
#include "Structs.h"
#include "Entity/BaseEntity.h"
#include "Entity/Shapes.h"

#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"

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

	void beginFrame();
	void endFrame();
	void Clear(Color c = {0.0f, 0.0f, 0.0f, 1.0f});

	// Entity
	void addEntity(BaseEntity* entity);
	void drawEntity(BaseEntity* entity);

	// Light
	void addLight(PointLight* light, bool activate);
	void activateLight(PointLight* light);
	
	// Camera
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