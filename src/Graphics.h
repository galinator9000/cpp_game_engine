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
#include "Graphics/Texture.h"
#include "Graphics/TextureSampler.h"

#include "Config.h"
#include "Camera.h"
#include "Timer.h"
#include "Structs.h"
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
	void setDefault();

	//// Shaders.
	void createVertexShader(VertexShader* shader, bool setShader = false);
	void setVertexShader(VertexShader* vertexShader);

	void createPixelShader(PixelShader* shader, bool setShader = false);
	void setPixelShader(PixelShader* pixelShader);
	void setTexturePixelShader(unsigned int slot, Texture* texture);
	void setTextureSamplerPixelShader(unsigned int slot, TextureSampler* textureSampler);

	// Buffer
	void bindVertexShaderBuffer(unsigned int slot, ID3D11Buffer* pBuffer);
	void bindPixelShaderBuffer(unsigned int slot, ID3D11Buffer* pBuffer);

	//// Viewports.
	void setViewport(Viewport* viewPort);

	//// Render targets.
	void createRenderTarget(RenderTarget* renderTarget, bool isShaderResource = false, ID3D11Resource* pTargetResource = NULL, bool setRenderTarget = false);
	void setRenderTarget(RenderTarget* renderTarget);
	void clearStateRenderTarget(RenderTarget* renderTarget, Color c = { 0.0f, 0.0f, 0.0f, 1.0f });
	std::vector<RenderTarget*> renderTargets;

	////// GAME ENGINE SECTION
	// Entity
	bool addEntity(Entity* entity);
	void drawEntity(Entity* entity);
	void updateEntity(Entity* entity);

	// Light
	bool createLightsBuffer(LightPSStruct* gAllLights, unsigned int lightCount, wrl::ComPtr<ID3D11Buffer>* pAllLights);
	void updateLightsBuffer(LightPSStruct* gAllLights, unsigned int lightCount, ID3D11Buffer* pAllLights);
	// Shadow
	bool createShadowMapsBuffer(ShadowMapSStruct* gAllShadowMaps, unsigned int shadowMapCount, wrl::ComPtr<ID3D11Buffer>* pAllShadowMaps);
	void updateShadowMapsBuffer(ShadowMapSStruct* gAllShadowMaps, unsigned int shadowMapCount, ID3D11Buffer* pAllShadowMaps);
	
	// Camera
	bool addCamera(Camera* camera, bool setAsMain);
	void activateCamera(Camera* camera);
	void updateCamera(Camera* camera);

	// Texturing
	void setTexture(Texture* texture);
	bool createTextureDDS(Texture* texture);
	bool createTextureSampler(TextureSampler* textureSampler);

	//// Graphics components
	GraphicsSettingsStruct gGraphicsSettingsStruct;
	wrl::ComPtr<ID3D11Buffer> pGraphicsSettingsStruct;

	// Main render target.
	RenderTarget* mainRenderTarget;
	Viewport* viewPort;

	// Samplers
	TextureSampler* defaultSampler;
	TextureSampler* clampSampler;
	TextureSampler* borderSampler;

	// Shaders.
	VertexShader* mainVertexShader = new VertexShader(L"VertexShader.cso");
	PixelShader* mainPixelShader = new PixelShader(L"PixelShader.cso");
	VertexShader* depthVertexShader = new VertexShader(L"DepthVS.cso");
	PixelShader* depthPixelShader = new PixelShader(L"DepthPS.cso");

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
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
};