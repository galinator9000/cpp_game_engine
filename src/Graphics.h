#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DirectXTK.lib")
#include <d3d11.h>
#include "d3dcompiler.h"
#include <DirectXMath.h>
#include <wrl.h>
#include <iostream>
#include <vector>
#include "DDSTextureLoader.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

#include "Graphics/Shader.h"
#include "Graphics/RenderTarget.h"
#include "Config.h"
#include "Camera.h"
#include "Timer.h"
#include "Structs.h"
#include "Texture.h"
#include "TextureSampler.h"
#include "Entity/Entity.h"
#include "Entity/Mesh/Mesh.h"
#include "Light/Light.h"

// Macro for handling HRESULT graphics errors from Direct3D API calls.
#define GFX_ERROR_CHECK(hr)

class Graphics{
public:
	////// DIRECTX SETUP AND GENERAL FUNCTIONS SECTION
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

	//// Shaders.
	void createVertexShader(VertexShader* shader, bool setShader = false);
	void setVertexShader(VertexShader* vertexShader);
	void createPixelShader(PixelShader* shader, bool setShader = false);
	void setPixelShader(PixelShader* pixelShader);
	// Main (or default) shader pointers.
	VertexShader* mainVertexShader = new VertexShader(L"VertexShader.cso");
	PixelShader* mainPixelShader = new PixelShader(L"PixelShader.cso");
	// Depth shaders.
	VertexShader* depthVertexShader = new VertexShader(L"DepthVS.cso");
	PixelShader* depthPixelShader = new PixelShader(L"DepthPS.cso");

	//// Render targets.
	void createRenderTarget(RenderTarget* renderTarget, ID3D11Resource* pBackBufferRes=NULL, bool setRenderTarget = false);
	void setRenderTarget(RenderTarget* renderTarget);
	void clearStateRenderTarget(RenderTarget* renderTarget, Color c = { 0.0f, 0.0f, 0.0f, 1.0f });
	std::vector<RenderTarget*> renderTargets;
	// Main (or default) render target pointer.
	RenderTarget* mainRenderTarget = new RenderTarget();
	// Depth render target pointer.
	RenderTarget* depthRenderTarget = new RenderTarget();

	////// GAME ENGINE SECTION
	// Entity
	bool addEntity(Entity* entity);
	void drawEntity(Entity* entity);
	void updateEntity(Entity* entity);

	// Light
	bool createLightsBuffer(LightPSConstantBuffer* gAllLights, unsigned int lightCount, wrl::ComPtr<ID3D11Buffer>* pAllLights);
	void updateLightsBuffer(LightPSConstantBuffer* gAllLights, unsigned int lightCount, ID3D11Buffer* pAllLights);
	void bindLightsBuffer(ID3D11Buffer* pAllLights);
	
	// Camera
	bool addCamera(Camera* camera, bool setAsMain);
	void activateCamera(Camera* camera);
	void updateCamera(Camera* camera);

	// Texturing
	bool createTextureDDS(Texture* texture);
	bool createTextureSampler(TextureSampler* textureSampler);

	unsigned int gWidth;
	unsigned int gHeight;

	// Timer instance for calculating FPS.
	Timer gTimer;
	unsigned short gFrameCounter = 0;
	unsigned int gCurrentFPS = 0;
private:
	HRESULT hr;

	// Device side
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;

	// Pipeline
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};