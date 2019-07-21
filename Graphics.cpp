#include "Graphics.h"

namespace wrl = Microsoft::WRL;

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

	// Create a RenderTargetView for rendering.
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	this->hr = this->pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	this->hr = this->pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		NULL,
		&this->pRenderTargetView
	);

	// Bind RenderTargetView to end of the pipeline.
	this->pDeviceContext->OMSetRenderTargets(
		1,
		this->pRenderTargetView.GetAddressOf(),
		NULL
	);

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
}

// Clears target view with specified RGBA color, if not specified, does it with black color.
void Graphics::Clear(float r, float g, float b, float a){
	this->pDeviceContext->ClearRenderTargetView(
		pRenderTargetView.Get(),
		new float[4]{r, g, b, a}
	);
}

void Graphics::BeginFrame(){
	this->Clear(0.3, 0.6, 0.9, 1.0);
}

void Graphics::EndFrame() {
	// Build vertex data.
	struct Vertex {
		float x;
		float y;
	};
	const Vertex vertices[] = {
		{ 0.0f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f },
	};

	// Create VertexBuffer.
	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(vertices);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	this->hr = this->pDevice->CreateBuffer(
		&bd,
		&sd,
		&pVertexBuffer
	);

	// Bind VertexBuffer to pipeline.
	const UINT pStrides = sizeof(Vertex);
	const UINT pOffsets = 0u;
	this->pDeviceContext->IASetVertexBuffers(
		0u,
		1u,
		pVertexBuffer.GetAddressOf(),
		&pStrides,
		&pOffsets
	);

	// Set Input Layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	//D3DReadFileToBlob(L"VertexShader.cso", &this->pBlob);
	this->hr = this->pDevice->CreateInputLayout(
		ied,
		1,
		this->pBlob->GetBufferPointer(),
		this->pBlob->GetBufferSize(),
		&pInputLayout
	);
	this->pDeviceContext->IASetInputLayout(pInputLayout.Get());

	// Set primitive topology
	this->pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw it..
	this->pDeviceContext->Draw(3, 0);

	this->hr = this->pSwapChain->Present(1, 0);
}