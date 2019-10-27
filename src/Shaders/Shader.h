#pragma once
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;

const enum SHADER_TYPE {
	VERTEX_SHADER,
	PIXEL_SHADER
};

class Shader {
public:
	LPCWSTR fileName;
	SHADER_TYPE type;
};

class VertexShader : public Shader {
public:
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;

	VertexShader(LPCWSTR fileName) {
		this->type = VERTEX_SHADER;
		this->fileName = fileName;
	}
};

class PixelShader : public Shader {
public:
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;

	PixelShader(LPCWSTR fileName) {
		this->type = PIXEL_SHADER;
		this->fileName = fileName;
	}
};