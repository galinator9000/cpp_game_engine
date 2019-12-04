#pragma once
#include "Camera.h"
#include "../Graphics/RenderTarget.h"
#include "../Graphics/Viewport.h"

class ShadowMap {
public:
	ShadowMap(
		Vector3 position,
		Vector3 direction,
		Vector2 dimensions,
		PROJECTION_TYPE projectionType
	) {
		this->pCamera = new Camera(position, direction, WIDTH, HEIGHT, projectionType);
		this->pRenderTarget = new RenderTarget();
		this->pViewPort = new Viewport({ dimensions.x, dimensions.y });

		this->activeCameraSubfrustumNearPlaneDistance = 0;
		this->activeCameraSubfrustumFarPlaneDistance = 0;
	}

	Camera* pCamera = NULL;
	RenderTarget* pRenderTarget = NULL;
	Viewport* pViewPort = NULL;
	float activeCameraSubfrustumNearPlaneDistance = NULL;
	float activeCameraSubfrustumFarPlaneDistance = NULL;
};