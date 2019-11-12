#pragma once
#include "Camera.h"
#include "../Graphics/RenderTarget.h"
#include "../Graphics/Viewport.h"

class ShadowMap {
public:
	ShadowMap(
		Camera* cam,
		RenderTarget* rt,
		Viewport* vp
	) {
		this->pCamera = cam;
		this->pRenderTarget = rt;
		this->pViewPort = vp;
	}

	Camera* pCamera = NULL;
	RenderTarget* pRenderTarget = NULL;
	Viewport* pViewPort = NULL;
};