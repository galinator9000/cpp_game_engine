#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;

class RenderTarget{
public:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pBackBufferRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSView;
};