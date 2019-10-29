#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;

class Shader {
public:
	LPCWSTR fileName;
};

class VertexShader : public Shader {
public:
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;

	VertexShader(LPCWSTR fileName) {
		this->fileName = fileName;
	}
};

class PixelShader : public Shader {
public:
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;

	PixelShader(LPCWSTR fileName) {
		this->fileName = fileName;
	}
};