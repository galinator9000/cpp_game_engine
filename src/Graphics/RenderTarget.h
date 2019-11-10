#pragma once
#include "Texture.h"
#include "TextureSampler.h"

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;

class RenderTarget{
public:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthView;

	// For sampling values from DepthView object.
	Texture* pTexture = new Texture();
};