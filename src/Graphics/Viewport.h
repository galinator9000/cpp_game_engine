#pragma once
#include <d3d11.h>

#include "Structs.h"

class Viewport {
public:
	Viewport() {
		this->viewPortStruct.Width = WIDTH;
		this->viewPortStruct.Height = HEIGHT;
		this->viewPortStruct.MinDepth = 0;
		this->viewPortStruct.MaxDepth = 1;
		this->viewPortStruct.TopLeftX = 0;
		this->viewPortStruct.TopLeftY = 0;
	}

	Viewport(Vector2 dimensions, float minDepth = 0, float maxDepth = 1) {
		if (dimensions.minimum() > 0) {
			this->viewPortStruct.Width = float(dimensions.x);
			this->viewPortStruct.Height = float(dimensions.y);
			this->viewPortStruct.MinDepth = minDepth;
			this->viewPortStruct.MaxDepth = maxDepth;
			this->viewPortStruct.TopLeftX = 0;
			this->viewPortStruct.TopLeftY = 0;
		}
	}

	D3D11_VIEWPORT viewPortStruct = {};
};