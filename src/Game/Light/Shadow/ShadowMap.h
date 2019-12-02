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
		PROJECTION_TYPE projectionType,
		unsigned int subFrustumCount=1
	) {
		this->pCamera = new Camera*[subFrustumCount];
		this->pRenderTarget = new RenderTarget*[subFrustumCount];
		this->pViewPort = new Viewport*[subFrustumCount];
		this->activeCameraSubfrustumNearPlaneDistance = new float[subFrustumCount];
		this->activeCameraSubfrustumFarPlaneDistance = new float[subFrustumCount];
		this->subFrustumCount = subFrustumCount;

		if (subFrustumCount > 1) {
			this->isCascaded = true;

			for (unsigned sf = 0; sf < subFrustumCount; sf++) {
				this->pCamera[sf] = new Camera(position, direction, WIDTH, HEIGHT, projectionType);
				this->pRenderTarget[sf] = new RenderTarget();
				this->pViewPort[sf] = new Viewport({ dimensions.x, dimensions.y });

				this->activeCameraSubfrustumNearPlaneDistance[sf] = 0;
				this->activeCameraSubfrustumFarPlaneDistance[sf] = 0;
			}
		}
		else {
			this->pCamera[0] = new Camera(position, direction, WIDTH, HEIGHT, projectionType);
			this->pRenderTarget[0] = new RenderTarget();
			this->pViewPort[0] = new Viewport({ dimensions.x, dimensions.y });

			this->activeCameraSubfrustumNearPlaneDistance[0] = 0;
			this->activeCameraSubfrustumFarPlaneDistance[0] = 0;
		}
	}

	Camera** pCamera = NULL;
	RenderTarget** pRenderTarget = NULL;
	Viewport** pViewPort = NULL;
	float* activeCameraSubfrustumNearPlaneDistance = NULL;
	float* activeCameraSubfrustumFarPlaneDistance = NULL;
	unsigned subFrustumCount = 1;
	bool isCascaded = false;
};