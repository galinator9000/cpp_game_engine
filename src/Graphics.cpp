#include "Graphics.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

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

	// Create D3D device & swap chain.
	this->hr = D3D11CreateDeviceAndSwapChain(
		NULL,								// pAdapter
		D3D_DRIVER_TYPE_HARDWARE,			// DriverType
		NULL,								// Software

		// Flags
#ifndef _DEBUGGING
		D3D11_CREATE_DEVICE_DEBUG,
#else
		0,
#endif
		FeatureLevelsRequested,				// pFeatureLevels
		6,									// FeatureLevels count
		D3D11_SDK_VERSION,					// SDKVersion
		&scd,								// pSwapChainDesc
		&pSwapChain,						// ppSwapChain
		&pDevice,							// ppDevice
		&FeatureLevelsSupported,			// pFeatureLevel
		&pDeviceContext						// ppImmediateContext
	);

	// Create a RenderTargetView for rendering.
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	this->hr = this->pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	this->hr = this->pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		NULL,
		&this->pRenderTargetView
	);

	// Bind RenderTargetView to end of the pipeline.
	/*this->pDeviceContext->OMSetRenderTargets(
		1,
		this->pRenderTargetView.GetAddressOf(),
		NULL
	);*/

	// Bind Pixel Shader to pipeline.
	this->hr = D3DReadFileToBlob(L"PixelShader.cso", &this->pBlob);
	this->hr = this->pDevice->CreatePixelShader(
		this->pBlob->GetBufferPointer(),
		this->pBlob->GetBufferSize(),
		NULL,
		&this->pPixelShader
	);
	this->pDeviceContext->PSSetShader(
		pPixelShader.Get(),
		NULL,
		0
	);

	// Bind Vertex Shader to pipeline.
	this->hr = D3DReadFileToBlob(L"VertexShader.cso", &this->pBlob);
	this->hr = this->pDevice->CreateVertexShader(
		this->pBlob->GetBufferPointer(),
		this->pBlob->GetBufferSize(),
		NULL,
		&this->pVertexShader
	);
	this->pDeviceContext->VSSetShader(
		pVertexShader.Get(),
		NULL,
		0
	);

	// Create a viewport configuration & bind it to pipeline.
	D3D11_VIEWPORT vp;
	vp.Width = float(WIDTH);
	vp.Height = float(HEIGHT);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	this->pDeviceContext->RSSetViewports(1, &vp);

	// Create depth buffer & bind it to pipeline. (a.k.a Z-Buffer)
	CD3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	this->hr = this->pDevice->CreateDepthStencilState(&dsDesc, &pDSState);
	this->pDeviceContext->OMSetDepthStencilState(pDSState.Get(), 1);

	// Create 2D texture for Depth Buffer.
	wrl::ComPtr<ID3D11Texture2D> pDSTXT;
	D3D11_TEXTURE2D_DESC descDSTXT = {};
	descDSTXT.Width = WIDTH;
	descDSTXT.Height = HEIGHT;
	descDSTXT.MipLevels = 1;
	descDSTXT.ArraySize = 1;
	descDSTXT.Format = DXGI_FORMAT_D32_FLOAT;
	descDSTXT.SampleDesc.Count = 1;
	descDSTXT.SampleDesc.Quality = 0;
	descDSTXT.Usage = D3D11_USAGE_DEFAULT;
	descDSTXT.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	this->hr = this->pDevice->CreateTexture2D(&descDSTXT, NULL, &pDSTXT);

	// Create DepthStencil view object.
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	this->pDevice->CreateDepthStencilView(pDSTXT.Get(), &descDSV, &pDSView);

	// Set DepthStencilView as render target.
	this->pDeviceContext->OMSetRenderTargets(1, this->pRenderTargetView.GetAddressOf(), pDSView.Get());
}

// Clears target view with specified RGBA color, if not specified, does it with black color.
void Graphics::Clear(float r, float g, float b, float a){
	// Clear back buffer.
	this->pDeviceContext->ClearRenderTargetView(
		this->pRenderTargetView.Get(),
		new float[4]{r, g, b, a}
	);

	// Clear z-buffer.
	this->pDeviceContext->ClearDepthStencilView(
		this->pDSView.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0u
	);
}

void Graphics::BeginFrame(){
	this->Clear();
}

void Graphics::EndFrame(){
	this->hr = this->pSwapChain->Present(1, 0);
}

void Graphics::AddEntity(BaseEntity& entity){
	// Create box vertices and indices for rendering the entity.
	switch (entity.type) {
		case ENTITY_TYPE::BOX:
			break;
		case ENTITY_TYPE::PLANE:
			break;
	}

	// Build constant buffer.
	const dx::XMMATRIX constBuffer[] = {
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ(entity.gRotation.z) *
			dx::XMMatrixRotationY(entity.gRotation.y) *
			dx::XMMatrixRotationX(entity.gRotation.x) *
			dx::XMMatrixTranslation(entity.gPosition.x, entity.gPosition.y, entity.gPosition.z) *
			dx::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 10.0f)
		)
	};

	// Create buffer.
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.ByteWidth = sizeof(constBuffer);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = { &constBuffer, 0, 0 };
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	this->hr = this->pDevice->CreateBuffer(
		&cbd,
		&csd,
		&pConstantBuffer
	);

	// Bind buffer to pipeline.
	this->pDeviceContext->VSSetConstantBuffers(
		0,
		1,
		pConstantBuffer.GetAddressOf()
	);
}

void Graphics::DrawEntity(BaseEntity* entity){

}