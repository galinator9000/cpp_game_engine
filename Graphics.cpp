#include "Graphics.h"

Graphics::Graphics(HWND hWnd, int WIDTH, int HEIGHT, int REFRESH_RATE){
	// Initialize graphics object, create Direct3D device.
	DXGI_SWAP_CHAIN_DESC scd;

	scd.BufferDesc.Width = WIDTH;
	scd.BufferDesc.Height = HEIGHT;
	scd.BufferDesc.RefreshRate.Numerator = REFRESH_RATE;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	const D3D_FEATURE_LEVEL FeatureLevelsRequested[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	const UINT FeatureLevelsRequestedSize = sizeof(FeatureLevelsRequested);

	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	HRESULT hrCreateDevice;
	hrCreateDevice = D3D11CreateDeviceAndSwapChain(
		NULL,								// pAdapter
		D3D_DRIVER_TYPE_HARDWARE,			// DriverType
		NULL,								// Software
		0,									// Flags
		FeatureLevelsRequested,				// pFeatureLevels
		6,									// FeatureLevels count
		D3D11_SDK_VERSION,					// SDKVersion
		&scd,								// pSwapChainDesc
		&pSwapChain,						// ppSwapChain
		&pDevice,							// ppDevice
		&FeatureLevelsSupported,			// pFeatureLevel
		&pDeviceContext						// ppImmediateContext
	);

	ID3D11Resource* pBackBuffer;
	this->pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	this->pDevice->CreateRenderTargetView(
		pBackBuffer,
		NULL,
		&this->pRenderTargetView
	);
	pBackBuffer->Release();
}

// Clears target view with black color.
void Graphics::Clear() {
	this->pDeviceContext->ClearRenderTargetView(
		this->pRenderTargetView,
		new float[4]{ 0.0, 0.0, 0.0, 1.0 }
	);
}

// Clears target view with specified RGBA color.
void Graphics::Clear(float r, float g, float b, float a){
	this->pDeviceContext->ClearRenderTargetView(
		this->pRenderTargetView,
		new float[4]{r, g, b, a}
	);
}

void Graphics::BeginFrame(){
	this->Clear();
}

void Graphics::EndFrame(){
	this->pSwapChain->Present(1, 0);
}