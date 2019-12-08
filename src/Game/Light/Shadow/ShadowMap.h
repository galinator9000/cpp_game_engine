#pragma once
#include "Camera.h"
#include "../Graphics/RenderTarget.h"
#include "../Graphics/Viewport.h"

class ShadowMap {
public:
	ShadowMap(
		Vector3 position,
		Vector3 direction,
		unsigned int width,
		unsigned int height,
		PROJECTION_TYPE projectionType
	) {
		this->pCamera = new Camera(position, direction, (float) width, (float) height, projectionType);
		this->pRenderTarget = new RenderTarget();
		this->pViewport = new Viewport({ (float) width, (float) height });
		this->pTexture = new Texture(width, height);

		this->activeCameraSubfrustumNearPlaneDistance = 0;
		this->activeCameraSubfrustumFarPlaneDistance = 0;
	}

	Camera* pCamera = NULL;
	RenderTarget* pRenderTarget = NULL;
	Viewport* pViewport = NULL;
	// Texture object to be filled with shadow map information.
	Texture* pTexture = NULL;

	float activeCameraSubfrustumNearPlaneDistance = NULL;
	float activeCameraSubfrustumFarPlaneDistance = NULL;
};