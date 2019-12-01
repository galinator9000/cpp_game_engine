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
		this->subFrustumCount = subFrustumCount;

		if (subFrustumCount > 1) {
			this->isCascaded = true;

			for (unsigned sfc = 0; sfc < subFrustumCount; sfc++) {
				this->pCamera[sfc] = new Camera(position, direction, WIDTH, HEIGHT, projectionType);
				this->pRenderTarget[sfc] = new RenderTarget();
				this->pViewPort[sfc] = new Viewport({ dimensions.x, dimensions.y });
			}
		}
		else {
			this->pCamera[0] = new Camera(position, direction, WIDTH, HEIGHT, projectionType);
			this->pRenderTarget[0] = new RenderTarget();
			this->pViewPort[0] = new Viewport({ dimensions.x, dimensions.y });
		}
	}

	Camera** pCamera = NULL;
	RenderTarget** pRenderTarget = NULL;
	Viewport** pViewPort = NULL;
	unsigned subFrustumCount = 1;
	bool isCascaded = false;
};