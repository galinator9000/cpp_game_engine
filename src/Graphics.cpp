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

	// Create a viewport configuration & bind it to pipeline.
	this->viewPort = new Viewport({ (float) WIDTH, (float) HEIGHT });
	this->setViewport(this->viewPort);

	// Get back-buffer from swap chain created by API call.
	wrl::ComPtr<ID3D11Texture2D> pBackBufferTexture;
	this->hr = this->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBufferTexture.GetAddressOf()));

	//// Render target creations.
	this->mainRenderTarget = new RenderTarget();
	this->createRenderTarget(this->mainRenderTarget, pBackBufferTexture.Get(), true, false, NULL);
	renderTargets.push_back(this->mainRenderTarget);

	//// Shader creation.
	// Create main shaders.
	this->createVertexShader(this->mainVertexShader, true);
	this->createPixelShader(this->mainPixelShader, true);
	// Create depth shaders.
	this->createVertexShader(this->depthVertexShader);
	this->createPixelShader(this->depthPixelShader);

	// Create graphics settings buffer.
	gGraphicsSettingsStruct.PCFLevel = 1;

	D3D11_BUFFER_DESC cBdPS = { 0 };
	cBdPS.ByteWidth = sizeof(this->gGraphicsSettingsStruct);
	cBdPS.Usage = D3D11_USAGE_DYNAMIC;
	cBdPS.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBdPS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA cSdPS = { &(this->gGraphicsSettingsStruct), 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBdPS,
		&cSdPS,
		&(this->pGraphicsSettingsStruct)
	);
	this->pDeviceContext->VSSetConstantBuffers(
		13,
		1,
		this->pGraphicsSettingsStruct.GetAddressOf()
	);
	this->pDeviceContext->PSSetConstantBuffers(
		13,
		1,
		this->pGraphicsSettingsStruct.GetAddressOf()
	);

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

	this->hr = D3DReadFileToBlob(this->mainVertexShader->fileName, &this->pBlob);
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

	// Create & set blend state for output-merger stage.
	D3D11_BLEND_DESC bDesc = {0};
	bDesc.IndependentBlendEnable = false;
	bDesc.RenderTarget[0].BlendEnable = true;
	bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	this->hr = this->pDevice->CreateBlendState(&bDesc, this->pBlendState.GetAddressOf());

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->pDeviceContext->OMSetBlendState(
		this->pBlendState.Get(),
		blendFactor,
		0xffffffff
	);

	//// Create & Set various Samplers
	D3D11_SAMPLER_DESC sampDesc = {};

	// Default (WRAP) sampler.
	this->defaultSampler = new TextureSampler();
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 0;
	this->hr = this->pDevice->CreateSamplerState(
		&sampDesc,
		&defaultSampler->pSamplerState
	);

	// Clamp sampler.
	sampDesc = {};
	this->clampSampler = new TextureSampler();
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 0;
	this->hr = this->pDevice->CreateSamplerState(
		&sampDesc,
		&clampSampler->pSamplerState
	);

	// Border sampler.
	sampDesc = {};
	this->borderSampler = new TextureSampler();
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.MipLODBias = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 0;

	// White color.
	sampDesc.BorderColor[0] = 1;
	sampDesc.BorderColor[1] = 1;
	sampDesc.BorderColor[2] = 1;
	sampDesc.BorderColor[3] = 1;

	this->hr = this->pDevice->CreateSamplerState(
		&sampDesc,
		&borderSampler->pSamplerState
	);

	this->setDefault();
}

// Clears target view with specified RGBA color, if not specified, does it with black color.
void Graphics::Clear(Color c) {
	for (RenderTarget* renderTarget : renderTargets) {
		if (renderTarget != NULL) {
			clearStateRenderTarget(renderTarget, c);
		}
	}
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

//// Viewports.
void Graphics::setViewport(Viewport* viewPort) {
	if (viewPort != NULL) {
		this->pDeviceContext->RSSetViewports(1, &viewPort->viewPortStruct);
	}
}

//// Render targets.
bool Graphics::createRenderTarget(RenderTarget* renderTarget, ID3D11Resource* pTargetResource, bool setRenderTarget, bool isShaderResource, Texture* pTexture) {
	// If resource provided, create render target view.
	if (isShaderResource && pTexture == NULL) {
		return false;
	}

	if (pTargetResource != NULL) {
		this->hr = this->pDevice->CreateRenderTargetView(
			pTargetResource,
			NULL,
			&renderTarget->pRenderTargetView
		);
	}

	//// Create depth view & state objects and bind it to pipeline. (a.k.a Z-Buffer)
	// Create Depth state.
	CD3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	this->hr = this->pDevice->CreateDepthStencilState(&dsDesc, &renderTarget->pDepthState);

	// Create Depth View.
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Flags = 0;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if (isShaderResource) {
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		this->hr = this->pDevice->CreateDepthStencilView(pTexture->pColorResource.Get(), &descDSV, &renderTarget->pDepthView);
	}
	else {
		// Create 2D texture for Depth View.
		wrl::ComPtr<ID3D11Texture2D> pDSTexture;
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
		this->hr = this->pDevice->CreateTexture2D(&descDSTXT, NULL, &pDSTexture);
		this->hr = this->pDevice->CreateDepthStencilView(pDSTexture.Get(), &descDSV, &renderTarget->pDepthView);
	}

	// Create Shader Resource View for providing DepthView to shader.
	if (isShaderResource) {
		// Shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC resViewDsc = {};
		resViewDsc.Format = DXGI_FORMAT_R32_FLOAT;
		resViewDsc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resViewDsc.Texture2D.MipLevels = 1;
		resViewDsc.Texture2D.MostDetailedMip = 0;

		this->hr = this->pDevice->CreateShaderResourceView(
			pTexture->pColorResource.Get(),
			&resViewDsc,
			&pTexture->pColorShaderResourceView
		);
	}

	if (setRenderTarget) {
		this->setRenderTarget(renderTarget);
	}

	return true;
}

void Graphics::setRenderTarget(RenderTarget* renderTarget) {
	if (renderTarget == NULL) {
		return;
	}

	// Set render target.
	if (renderTarget->pRenderTargetView.Get() != NULL) {
		this->pDeviceContext->OMSetRenderTargets(1, renderTarget->pRenderTargetView.GetAddressOf(), renderTarget->pDepthView.Get());
	}else {
		// Unbind current render target.
		this->pDeviceContext->OMSetRenderTargets(0, NULL, renderTarget->pDepthView.Get());
	}

	// Set depth state.
	if (renderTarget->pDepthState.Get() != NULL) {
		this->pDeviceContext->OMSetDepthStencilState(renderTarget->pDepthState.Get(), 1);
	}
}

void Graphics::clearStateRenderTarget(RenderTarget* renderTarget, Color c) {
	if (renderTarget->pRenderTargetView != NULL) {
		// Clear back buffer.
		this->pDeviceContext->ClearRenderTargetView(
			renderTarget->pRenderTargetView.Get(),
			new float[4]{ c.r, c.g, c.b, c.a }
		);
	}
	if (renderTarget->pDepthView != NULL) {
		// Clear depth buffer.
		this->pDeviceContext->ClearDepthStencilView(
			renderTarget->pDepthView.Get(),
			D3D11_CLEAR_DEPTH,
			1.0f,
			0u
		);
	}
}

//// Shaders
void Graphics::createVertexShader(VertexShader* vertexShader, bool setShader) {
	this->hr = D3DReadFileToBlob(vertexShader->fileName, &this->pBlob);
	this->hr = this->pDevice->CreateVertexShader(
		this->pBlob->GetBufferPointer(),
		this->pBlob->GetBufferSize(),
		NULL,
		&vertexShader->pVertexShader
	);

	if (setShader) {
		this->setVertexShader(vertexShader);
	}
}
void Graphics::setVertexShader(VertexShader* vertexShader) {
	ID3D11VertexShader* pD3D11VertexShader = NULL;
	if (vertexShader != NULL) {
		pD3D11VertexShader = vertexShader->pVertexShader.Get();
	}

	this->pDeviceContext->VSSetShader(
		pD3D11VertexShader,
		NULL,
		0
	);
}

void Graphics::createPixelShader(PixelShader* pixelShader, bool setShader) {
	this->hr = D3DReadFileToBlob(pixelShader->fileName, &this->pBlob);
	this->hr = this->pDevice->CreatePixelShader(
		this->pBlob->GetBufferPointer(),
		this->pBlob->GetBufferSize(),
		NULL,
		&pixelShader->pPixelShader
	);

	if (setShader) {
		this->setPixelShader(pixelShader);
	}
}
void Graphics::setPixelShader(PixelShader* pixelShader) {
	ID3D11PixelShader* pD3D11PixelShader = NULL;
	if (pixelShader != NULL) {
		pD3D11PixelShader = pixelShader->pPixelShader.Get();
	}

	this->pDeviceContext->PSSetShader(
		pD3D11PixelShader,
		NULL,
		0
	);
}

void Graphics::setTexturePixelShader(unsigned int slot, Texture* texture) {
	if (texture == NULL || texture->pColorShaderResourceView.Get() == NULL) {
		return;
	}

	this->pDeviceContext->PSSetShaderResources(
		slot,
		1,
		texture->pColorShaderResourceView.GetAddressOf()
	);
}

void Graphics::setTextureSamplerPixelShader(unsigned int slot, TextureSampler* textureSampler) {
	if (textureSampler == NULL || textureSampler->pSamplerState.Get() == NULL) {
		return;
	}

	this->pDeviceContext->PSSetSamplers(
		slot,
		1,
		textureSampler->pSamplerState.GetAddressOf()
	);
}

void Graphics::bindVertexShaderBuffer(unsigned int slot, ID3D11Buffer* pBuffer) {
	this->pDeviceContext->VSSetConstantBuffers(
		slot,
		1,
		&pBuffer
	);
}

void Graphics::bindPixelShaderBuffer(unsigned int slot, ID3D11Buffer* pBuffer) {
	this->pDeviceContext->PSSetConstantBuffers(
		slot,
		1,
		&pBuffer
	);
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
	// Clear texture slots.
	ID3D11ShaderResourceView* nullShaderResourceView = { nullptr };
	this->pDeviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);
	this->pDeviceContext->PSSetShaderResources(1, 1, &nullShaderResourceView);

	// Set samplers to default.
	this->setDefault();

	// Bind texture sampler if this entity uses a different one.
	if (entity->useTexture) {
		if (entity->textureSampler != NULL) {
			this->setTextureSamplerPixelShader(0, entity->textureSampler);
		}

		this->setTexture(entity->texture);
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
bool Graphics::createLightsBuffer(LightPSStruct* gAllLights, unsigned int lightCount, wrl::ComPtr<ID3D11Buffer>* pAllLights) {
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
	return true;
}

// Update buffer that contains multiple lights at the same time.
void Graphics::updateLightsBuffer(LightPSStruct* gAllLights, unsigned int lightCount, ID3D11Buffer* pAllLights) {
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

// Shadow Maps
// Create buffers that will contain shadow maps.
bool Graphics::createShadowBoxesBuffer(ShadowBoxSStruct* gAllShadowBoxes, unsigned int shadowBoxCount, wrl::ComPtr<ID3D11Buffer>* pAllShadowBoxes) {
	D3D11_BUFFER_DESC cBd = { 0 };
	cBd.ByteWidth = (unsigned int)(sizeof(*gAllShadowBoxes) * shadowBoxCount);
	cBd.Usage = D3D11_USAGE_DYNAMIC;
	cBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA cSd = { gAllShadowBoxes, 0, 0 };
	this->hr = this->pDevice->CreateBuffer(
		&cBd,
		&cSd,
		pAllShadowBoxes->GetAddressOf()
	);
	return true;
}
void Graphics::updateShadowBoxesBuffer(ShadowBoxSStruct* gAllShadowBoxes, unsigned int shadowBoxCount, ID3D11Buffer* pAllShadowBoxes) {
	D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
	this->hr = this->pDeviceContext->Map(
		pAllShadowBoxes,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	memcpy(mappedResource.pData, gAllShadowBoxes, (unsigned int)(sizeof(*gAllShadowBoxes) * shadowBoxCount));
	this->pDeviceContext->Unmap(pAllShadowBoxes, 0);
}

bool Graphics::createShadowMapTexture(Texture* texture) {
	ID3D11Texture2D* pDSTexture;
	D3D11_TEXTURE2D_DESC descDSTXT = {};
	descDSTXT.Width = texture->gWidth;
	descDSTXT.Height = texture->gHeight;
	descDSTXT.MipLevels = 1;
	descDSTXT.ArraySize = 1;
	descDSTXT.Format = DXGI_FORMAT_R32_TYPELESS;
	descDSTXT.SampleDesc.Count = 1;
	descDSTXT.SampleDesc.Quality = 0;
	descDSTXT.Usage = D3D11_USAGE_DEFAULT;
	descDSTXT.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	this->hr = this->pDevice->CreateTexture2D(&descDSTXT, NULL, &pDSTexture);
	texture->pColorResource = pDSTexture;
	return true;
}

// Camera
bool Graphics::addCamera(Camera* camera, bool setAsMain) {
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
	return true;
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
bool Graphics::createTextureDDS(Texture* texture) {
	this->hr = CreateDDSTextureFromFile(
		this->pDevice.Get(),
		this->pDeviceContext.Get(),
		texture->colorFileName.c_str(),
		texture->pColorResource.GetAddressOf(),
		texture->pColorShaderResourceView.GetAddressOf()
	);

	if (texture->useNormalMapping) {
		this->hr = CreateDDSTextureFromFile(
			this->pDevice.Get(),
			this->pDeviceContext.Get(),
			texture->normalFileName.c_str(),
			texture->pNormalResource.GetAddressOf(),
			texture->pNormalShaderResourceView.GetAddressOf()
		);
	}

	if (texture->useAlpha) {
		this->hr = CreateDDSTextureFromFile(
			this->pDevice.Get(),
			this->pDeviceContext.Get(),
			texture->alphaFileName.c_str(),
			texture->pAlphaResource.GetAddressOf(),
			texture->pAlphaShaderResourceView.GetAddressOf()
		);
	}
	return true;
}

void Graphics::setTexture(Texture* texture) {
	if (texture == NULL || texture->pColorShaderResourceView.Get() == NULL) {
		return;
	}

	this->pDeviceContext->PSSetShaderResources(
		0,
		1,
		texture->pColorShaderResourceView.GetAddressOf()
	);

	if (texture->useNormalMapping) {
		this->pDeviceContext->PSSetShaderResources(
			1,
			1,
			texture->pNormalShaderResourceView.GetAddressOf()
		);
	}

	if (texture->useAlpha) {
		this->pDeviceContext->PSSetShaderResources(
			2,
			1,
			texture->pAlphaShaderResourceView.GetAddressOf()
		);
	}
}

void Graphics::setDefault() {
	this->setTextureSamplerPixelShader(0, this->defaultSampler);
	this->setTextureSamplerPixelShader(1, this->clampSampler);
	this->setTextureSamplerPixelShader(2, this->borderSampler);
}