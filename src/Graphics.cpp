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
		{"TextureUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Binormal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

		// Per-Vertex joint information, maximum 4 joint supported.
		{"JointWeights", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JointIDs", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
		this->gCurrentFPS = (unsigned int) ((float) this->gFrameCounter / this->gTimer.Peek());

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
bool Graphics::addEntity(Entity* entity){
	if (entity->mesh == NULL || !entity->isDrawable) {
		return false;
	}

	/// CONSTANT BUFFER	
	// Create constant buffer on GPU side for Vertex Shader.
	D3D11_BUFFER_DESC cBdVS = { 0 };
	cBdVS.ByteWidth = sizeof(entity->gEntityVSConstantBuffer);
	cBdVS.Usage = D3D11_USAGE_DYNAMIC;
	cBdVS.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBdVS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA cSdVS = { &(entity->gEntityVSConstantBuffer), 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBdVS,
		&cSdVS,
		&(entity->pEntityVSConstantBuffer)
	);

	D3D11_BUFFER_DESC cBdPS = { 0 };
	cBdPS.ByteWidth = sizeof(entity->gEntityPSConstantBuffer);
	cBdPS.Usage = D3D11_USAGE_DYNAMIC;
	cBdPS.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBdPS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA cSdPS = { &(entity->gEntityPSConstantBuffer), 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBdPS,
		&cSdPS,
		&(entity->pEntityPSConstantBuffer)
	);

	// If attached mesh doesn't have GPU buffers yet, create it.
	// Otherwise, use the one that exists there.
	// Create constant buffer on GPU side for Pixel Shader.
	if (!entity->mesh->hasGPUBuffers) {
		// Vertex and Index buffers provided by mesh object.
		/// VERTEX BUFFER
		// Build vertex buffer on GPU side.
		D3D11_BUFFER_DESC vBd = { 0 };
		vBd.StructureByteStride = sizeof(Vertex);
		vBd.ByteWidth = (UINT)vBd.StructureByteStride * entity->mesh->gVertexCount;
		vBd.Usage = D3D11_USAGE_DEFAULT;
		vBd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vSd = { entity->mesh->gVertices, 0, 0 };
		this->hr = this->pDevice->CreateBuffer(
			&vBd,
			&vSd,
			&(entity->mesh->pVertexBuffer)
		);

		/// INDEX BUFFER
		// Create index buffer on GPU side.
		D3D11_BUFFER_DESC iBd = { 0 };
		iBd.StructureByteStride = sizeof(unsigned int);
		iBd.ByteWidth = (UINT)iBd.StructureByteStride * entity->mesh->gIndexCount;
		iBd.Usage = D3D11_USAGE_DEFAULT;
		iBd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA iSd = { entity->mesh->gIndices, 0, 0 };
		this->hr = this->pDevice->CreateBuffer(
			&iBd,
			&iSd,
			&(entity->mesh->pIndexBuffer)
		);

		// Attached mesh has GPU buffers from now on.
		entity->mesh->hasGPUBuffers = true;
	}

	// If mesh deformer is attached to mesh, create buffers for it.
	if (entity->meshDeformer != NULL) {
		/// MESH DEFORMER CONSTANT BUFFER
		// Create constant buffer on GPU side for Vertex Shader.
		D3D11_BUFFER_DESC mdcBdVS = { 0 };
		mdcBdVS.ByteWidth = sizeof(entity->meshDeformer->gMeshDeformerVSConstantBuffer);
		mdcBdVS.Usage = D3D11_USAGE_DYNAMIC;
		mdcBdVS.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		mdcBdVS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA mdcSdVS = { &(entity->meshDeformer->gMeshDeformerVSConstantBuffer), 0, 0 };
		this->hr = this->pDevice->CreateBuffer(
			&mdcBdVS,
			&mdcSdVS,
			&(entity->meshDeformer->pMeshDeformerVSConstantBuffer)
		);
	}

	return true;
}

void Graphics::drawEntity(Entity* entity){
	if (entity->mesh == NULL || !entity->isDrawable) {
		return;
	}

	//// Binding Buffers
	// Constant buffer
	// Bind entity's constant buffer for Vertex Shader.
	this->pDeviceContext->VSSetConstantBuffers(
		0,
		1,
		entity->pEntityVSConstantBuffer.GetAddressOf()
	);

	// Bind entity's constant buffer for Pixel Shader.
	this->pDeviceContext->PSSetConstantBuffers(
		0,
		1,
		entity->pEntityPSConstantBuffer.GetAddressOf()
	);

	// Vertex buffer
	const UINT pStrides = sizeof(Vertex);
	const UINT pOffsets = 0;
	this->pDeviceContext->IASetVertexBuffers(
		0,
		1,
		entity->mesh->pVertexBuffer.GetAddressOf(),
		&pStrides,
		&pOffsets
	);

	// Index buffer
	this->pDeviceContext->IASetIndexBuffer(
		entity->mesh->pIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);

	//// Binding Texture & Samplers
	// Clear texture & sampler slots.
	ID3D11ShaderResourceView* nullShaderResourceView = { nullptr };
	this->pDeviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);
	this->pDeviceContext->PSSetShaderResources(1, 1, &nullShaderResourceView);
	ID3D11SamplerState* nullSamplerState = { nullptr }; this->pDeviceContext->PSSetSamplers(0, 1, &nullSamplerState);

	// Bind sampler.
	if (entity->useTexture || entity->useNormalMapping) {
		// Bind entity's sampler state to first (index 0) sampler slot of the Pixel Shader.
		this->pDeviceContext->PSSetSamplers(
			0,
			1,
			entity->textureSampler->pSamplerState.GetAddressOf()
		);
	}

	// Bind texture resources, only if entity uses textures.
	if (entity->useTexture) {
		// Bind entity's shader resource view (texture) to first (index 0) resource slot of the Pixel Shader.
		this->pDeviceContext->PSSetShaderResources(
			0,
			1,
			entity->texture->pShaderResourceView.GetAddressOf()
		);
	}

	// Bind normal mapping texture resources, only if entity uses normal mapping.
	if (entity->useNormalMapping) {
		// Bind entity's shader resource view (normal mapping texture) to second (index 1) resource slot of the Pixel Shader.
		this->pDeviceContext->PSSetShaderResources(
			1,
			1,
			entity->normalMappingTexture->pShaderResourceView.GetAddressOf()
		);
	}

	// If mesh deformer is attached to mesh, set buffers to Vertex Shader.
	if (entity->meshDeformer != NULL) {
		// Bind entity's mesh's deformer buffer to third (index 2) slot of the Vertex Shader.
		this->pDeviceContext->VSSetConstantBuffers(
			2,
			1,
			entity->meshDeformer->pMeshDeformerVSConstantBuffer.GetAddressOf()
		);
	}

	// Draw the entity..
	this->pDeviceContext->DrawIndexed(
		(UINT) entity->mesh->gIndexCount,
		0,
		0
	);
}

void Graphics::updateEntity(Entity* entity) {
	if (entity->mesh == NULL || !entity->isDrawable) {
		return;
	}

	// Update subresource of the constant buffer on GPU side.
	// ONLY if it should.
	if (entity->shouldUpdateGPUData) {
		// Update constant buffer of entity for Vertex Shader.
		D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
		this->hr = this->pDeviceContext->Map(
			entity->pEntityVSConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		memcpy(mappedResource.pData, &entity->gEntityVSConstantBuffer, sizeof(entity->gEntityVSConstantBuffer));
		this->pDeviceContext->Unmap(entity->pEntityVSConstantBuffer.Get(), 0);

		// Update constant buffer of entity for Pixel Shader.
		mappedResource = { 0 };
		this->hr = this->pDeviceContext->Map(
			entity->pEntityPSConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		memcpy(mappedResource.pData, &entity->gEntityPSConstantBuffer, sizeof(entity->gEntityPSConstantBuffer));
		this->pDeviceContext->Unmap(entity->pEntityPSConstantBuffer.Get(), 0);
	}

	if (entity->meshDeformer != NULL) {
		if (entity->meshDeformer->shouldUpdateGPUData) {
			// Update constant buffer of the deformer of the entity for Vertex Shader.
			D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
			this->hr = this->pDeviceContext->Map(
				entity->meshDeformer->pMeshDeformerVSConstantBuffer.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedResource
			);
			memcpy(mappedResource.pData, &entity->meshDeformer->gMeshDeformerVSConstantBuffer, sizeof(entity->meshDeformer->gMeshDeformerVSConstantBuffer));
			this->pDeviceContext->Unmap(entity->meshDeformer->pMeshDeformerVSConstantBuffer.Get(), 0);
		}
	}
}

// Light
// Create buffers that will contain multiple lights at the same time.
void Graphics::createLightsBuffer(LightPSConstantBuffer* gAllLights, unsigned int lightCount, wrl::ComPtr<ID3D11Buffer>* pAllLights) {
	// Create buffer for holding light direction, position and intensity values on GPU side.
	D3D11_BUFFER_DESC cBd = { 0 };
	cBd.ByteWidth = (unsigned int) (sizeof(*gAllLights) * lightCount);
	cBd.Usage = D3D11_USAGE_DYNAMIC;
	cBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA cSd = { gAllLights, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBd,
		&cSd,
		pAllLights->GetAddressOf()
	);
}

// Update buffer that contains multiple lights at the same time.
void Graphics::updateLightsBuffer(LightPSConstantBuffer* gAllLights, unsigned int lightCount, ID3D11Buffer* pAllLights) {
	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	this->hr = this->pDeviceContext->Map(
		pAllLights,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	memcpy(mappedResource.pData, gAllLights, (unsigned int) (sizeof(*gAllLights) * lightCount));
	this->pDeviceContext->Unmap(pAllLights, 0);
}

// Bind buffer that contains all lights in world.
void Graphics::bindLightsBuffer(ID3D11Buffer* pAllLights) {
	this->pDeviceContext->PSSetConstantBuffers(
		1,
		1,
		&pAllLights
	);
}

// Camera
void Graphics::addCamera(Camera* camera, bool setAsMain) {
	// Create buffer for View and Projection matrices on GPU side.
	D3D11_BUFFER_DESC cBd = { 0 };
	cBd.ByteWidth = sizeof(camera->gCameraVSConstantBuffer);
	cBd.Usage = D3D11_USAGE_DYNAMIC;
	cBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA cSd = { &camera->gCameraVSConstantBuffer, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBd,
		&cSd,
		&(camera->pCameraVSConstantBuffer)
	);

	if (setAsMain) {
		this->activateCamera(camera);
	}
}

void Graphics::activateCamera(Camera* camera) {
	// Bind constant buffer that holds View and Projection matrices to second (index 1) slot of Vertex shader.
	this->pDeviceContext->VSSetConstantBuffers(
		1,
		1,
		camera->pCameraVSConstantBuffer.GetAddressOf()
	);
}

void Graphics::updateCamera(Camera* camera) {
	if (camera->shouldUpdateGPUData) {
		D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
		this->hr = this->pDeviceContext->Map(
			camera->pCameraVSConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		memcpy(mappedResource.pData, &camera->gCameraVSConstantBuffer, sizeof(camera->gCameraVSConstantBuffer));
		this->pDeviceContext->Unmap(camera->pCameraVSConstantBuffer.Get(), 0);
	}
}

// Texture
void Graphics::createTextureDDS(Texture* texture) {
	this->hr = CreateDDSTextureFromFile(
		this->pDevice.Get(),
		this->pDeviceContext.Get(),
		texture->fileName.c_str(),
		texture->pResource.GetAddressOf(),
		texture->pShaderResourceView.GetAddressOf()
	);
}

void Graphics::createTextureSampler(TextureSampler* textureSampler) {
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 0;

	this->hr = this->pDevice->CreateSamplerState(
		&sampDesc,
		&textureSampler->pSamplerState
	);
}