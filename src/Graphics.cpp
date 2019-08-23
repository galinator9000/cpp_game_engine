#include "Graphics.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

////// DIRECTX SETUP AND GENERAL FUNCTIONS SECTION

Graphics::Graphics(HWND hWnd, unsigned int WIDTH, unsigned int HEIGHT, int REFRESH_RATE){
	this->gWidth = WIDTH;
	this->gHeight = HEIGHT;

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

	// Create InputLayout
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	this->hr = D3DReadFileToBlob(L"VertexShader.cso", &this->pBlob);
	this->hr = this->pDevice->CreateInputLayout(
		ied,
		(UINT) std::size(ied),
		this->pBlob->GetBufferPointer(),
		this->pBlob->GetBufferSize(),
		&this->pInputLayout
	);

	// Set input layout and primitive topology.
	this->pDeviceContext->IASetInputLayout(this->pInputLayout.Get());
	this->pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// Clears target view with specified RGBA color, if not specified, does it with black color.
void Graphics::Clear(Color c) {
	// Clear back buffer.
	this->pDeviceContext->ClearRenderTargetView(
		this->pRenderTargetView.Get(),
		new float[4]{ c.r, c.g, c.b, c.a }
	);

	// Clear Z-buffer.
	this->pDeviceContext->ClearDepthStencilView(
		this->pDSView.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0u
	);
}

void Graphics::beginFrame() {
	this->Clear(BACKGROUND_COLOR);
}

void Graphics::endFrame() {
	this->gFrameCounter += 1;

	// Update this information every 250 millisecond.
	if (this->gTimer.Peek() > 0.25f) {
		// Calculate FPS.
		this->gCurrentFPS = (unsigned int) (float)this->gFrameCounter / this->gTimer.Peek();

		// Log current FPS.
		std::ostringstream myStream;
		myStream << "FPS: ";
		myStream << this->gCurrentFPS << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();

		// Reset counters.
		this->gTimer.Reset();
		this->gFrameCounter = 0;
	}

	this->hr = this->pSwapChain->Present(1, 0);
}

////// GAME ENGINE SECTION
// Entity
void Graphics::addEntity(BaseEntity* entity){
	switch (entity->type) {
		case ENTITY_TYPE::PLANE:
			return;
			break;
	}

	// Build vertex and index buffer elements depending on shape of the object,
	// fill them in the entity object's "indices" and "vertices" arrays.
	entity->gCreateVerticesAndIndices();

	/// CONSTANT BUFFER	
	// Create buffer on GPU side.
	D3D11_BUFFER_DESC cBd = { 0 };
	cBd.ByteWidth = sizeof(entity->gEntityConstBuffer);
	cBd.Usage = D3D11_USAGE_DYNAMIC;
	cBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBd.MiscFlags = 0;
	cBd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cSd = { &(entity->gEntityConstBuffer), 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBd,
		&cSd,
		&(entity->pEntityConstantBuffer)
	);

	/// VERTEX BUFFER
	// Build vertex buffer on GPU side.
	D3D11_BUFFER_DESC vBd = { 0 };
	vBd.StructureByteStride = sizeof(Vertex);
	vBd.ByteWidth = (UINT) vBd.StructureByteStride * entity->gVertexCount;
	vBd.Usage = D3D11_USAGE_DEFAULT;
	vBd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vSd = { entity->gVertices, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&vBd,
		&vSd,
		&(entity->pVertexBuffer)
	);

	/// INDEX BUFFER
	// Create index buffer on GPU side.
	D3D11_BUFFER_DESC iBd = { 0 };
	iBd.StructureByteStride = sizeof(unsigned int);
	iBd.ByteWidth = (UINT) iBd.StructureByteStride * entity->gIndexCount;
	iBd.Usage = D3D11_USAGE_DEFAULT;
	iBd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBd.CPUAccessFlags = 0u;
	iBd.MiscFlags = 0u;
	D3D11_SUBRESOURCE_DATA iSd = { entity->gIndices, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&iBd,
		&iSd,
		&(entity->pIndexBuffer)
	);
}

void Graphics::drawEntity(BaseEntity* entity){
	switch (entity->type) {
		case ENTITY_TYPE::PLANE:
			return;
			break;
	}

	// Bind buffers to pipeline.
	// Constant buffer

	// Update subresource of the constant buffer on GPU side.
	// ONLY if it should.
	if(entity->shouldUpdateData){
		D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
		this->pDeviceContext->Map(
			entity->pEntityConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		memcpy(mappedResource.pData, &entity->gEntityConstBuffer, sizeof(entity->gEntityConstBuffer));
		this->pDeviceContext->Unmap(entity->pEntityConstantBuffer.Get(), 0);
	}

	// Bind entity's constant buffer to first (index 0) slot.
	this->pDeviceContext->VSSetConstantBuffers(
		0,
		1,
		entity->pEntityConstantBuffer.GetAddressOf()
	);

	// Vertex buffer
	const UINT pStrides = sizeof(Vertex);
	const UINT pOffsets = 0;
	this->pDeviceContext->IASetVertexBuffers(
		0,
		1,
		entity->pVertexBuffer.GetAddressOf(),
		&pStrides,
		&pOffsets
	);

	// Index buffer
	this->pDeviceContext->IASetIndexBuffer(
		entity->pIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);

	// Draw the entity..
	this->pDeviceContext->DrawIndexed(
		(UINT) entity->gIndexCount,
		0,
		0
	);
}

// Light
void Graphics::addLight(Light* light, bool activate) {
	// Create buffer for holding light direction, position and intensity values on GPU side.
	D3D11_BUFFER_DESC cBd = { 0 };
	cBd.ByteWidth = sizeof(light->gLightConstBuffer);
	cBd.Usage = D3D11_USAGE_DYNAMIC;
	cBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBd.MiscFlags = 0;
	cBd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cSd = { &light->gLightConstBuffer, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBd,
		&cSd,
		&(light->pLightConstantBuffer)
	);

	if (activate) {
		this->activateLight(light);
	}
}

void Graphics::activateLight(Light* light) {
	// Bind constant buffer that holds light direction, position and intensity to first (index 0) slot of the Pixel Shader.
	this->pDeviceContext->PSSetConstantBuffers(
		0,
		1,
		light->pLightConstantBuffer.GetAddressOf()
	);
}

void Graphics::updateLight(Light* light) {
	if (light->shouldUpdateData) {
		D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
		this->pDeviceContext->Map(
			light->pLightConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		memcpy(mappedResource.pData, &light->gLightConstBuffer, sizeof(light->gLightConstBuffer));
		this->pDeviceContext->Unmap(light->pLightConstantBuffer.Get(), 0);
	}
}

// Camera
void Graphics::addCamera(Camera* camera, bool setAsMAin) {
	// Create buffer for View and Projection matrices on GPU side.
	D3D11_BUFFER_DESC cBd = { 0 };
	cBd.ByteWidth = sizeof(camera->gViewProjection);
	cBd.Usage = D3D11_USAGE_DYNAMIC;
	cBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBd.MiscFlags = 0;
	cBd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cSd = { &camera->gViewProjection, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBd,
		&cSd,
		&(camera->pViewProjectionBuffer)
	);

	if (setAsMAin) {
		this->setCamera(camera);
	}
}

void Graphics::setCamera(Camera* camera) {
	// Bind constant buffer that holds View and Projection matrices to second (index 1) slot of Vertex shader.
	this->pDeviceContext->VSSetConstantBuffers(
		1,
		1,
		camera->pViewProjectionBuffer.GetAddressOf()
	);
}

void Graphics::updateCamera(Camera* camera) {
	if (camera->shouldUpdateData) {
		D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
		this->pDeviceContext->Map(
			camera->pViewProjectionBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		memcpy(mappedResource.pData, &camera->gViewProjection, sizeof(camera->gViewProjection));
		this->pDeviceContext->Unmap(camera->pViewProjectionBuffer.Get(), 0);
	}
}