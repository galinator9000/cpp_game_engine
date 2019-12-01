#include "ShadowBox.h"

ShadowBox::ShadowBox(Vector3 position, Vector3 direction, Vector2 mapDimensions, LIGHT_TYPE lightType, unsigned int CSMLevel) {
	this->lightType = lightType;

	this->isActive = true;
	switch (this->lightType) {
		case LIGHT_TYPE::SPOT_LIGHT:
			if (CSMLevel > 1) {
				this->gShadowMap = new ShadowMap(
					position, direction, mapDimensions, PROJECTION_TYPE::PERSPECTIVE, CSMLevel
				);
			}
			else {
				this->gShadowMap = new ShadowMap(
					position, direction, mapDimensions, PROJECTION_TYPE::PERSPECTIVE
				);
			}
			break;

		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			if (CSMLevel > 1) {
				this->gShadowMap = new ShadowMap(
					Vector3(0, 0, 0), direction, mapDimensions, PROJECTION_TYPE::ORTHOGRAPHIC, CSMLevel
				);
			}
			else {
				this->gShadowMap = new ShadowMap(
					Vector3(0, 0, 0), direction, mapDimensions, PROJECTION_TYPE::ORTHOGRAPHIC
				);
			}
			break;
	}
}

void ShadowBox::Update(Vector3 position, Vector3 direction, Camera* activeCamera) {
	switch (this->lightType) {
		case LIGHT_TYPE::SPOT_LIGHT:
			if (this->gShadowMap->isCascaded) {
				for (unsigned sfc = 0; sfc < this->gShadowMap->subFrustumCount; sfc++) {
					this->gShadowMap->pCamera[sfc]->setPosition(position);
					this->gShadowMap->pCamera[sfc]->setDirection(direction);
				}
			}
			else {
				this->gShadowMap->pCamera[0]->setPosition(position);
				this->gShadowMap->pCamera[0]->setDirection(direction);
			}
			break;

		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			// Calculate active camera's frustum vertices.
			dx::BoundingFrustum activeCameraFrustum(
				dx::XMMatrixTranspose(
					dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.projectionMatrix)
				)
			);
			// Adjust far plane distance.
			this->gShadowDistance = (float)abs(activeCamera->farPlaneZ) * this->gShadowDistanceRatio;
			activeCameraFrustum.Far = gShadowDistance;

			dx::XMMATRIX activeCameraViewMatrix = dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.viewMatrix);
			dx::XMMATRIX activeCameraViewMatrixInverse = dx::XMMatrixInverse(
				NULL,
				dx::XMMatrixTranspose(
					activeCameraViewMatrix
				)
			);
			activeCameraFrustum.Transform(activeCameraFrustum, activeCameraViewMatrixInverse);

			// Get corner positions.
			dx::XMFLOAT3 activeCameraFrustumCorners[activeCameraFrustum.CORNER_COUNT];
			dx::XMFLOAT3 activeCameraFrustumCornersLightSpace[activeCameraFrustum.CORNER_COUNT];
			activeCameraFrustum.GetCorners(activeCameraFrustumCorners);

			// Calculate shadow box position and orientation with using active camera's frustum corner vertices.
			dx::XMMATRIX lightViewMatrix = dx::XMLoadFloat4x4(&this->gShadowMap->pCamera[0]->gCameraVSConstantBuffer.viewMatrix);
			dx::XMMATRIX lightViewMatrixInverse = dx::XMMatrixInverse(
				NULL,
				lightViewMatrix
			);

			// Transform frustum vertices from world space to light space.
			for (unsigned int cc = 0; cc < activeCameraFrustum.CORNER_COUNT; cc++) {
				dx::XMStoreFloat3(
					&activeCameraFrustumCornersLightSpace[cc],
					dx::XMVector3Transform(
						dx::XMLoadFloat3(&activeCameraFrustumCorners[cc]),
						lightViewMatrix
					)
				);
			}

			// Calculate dimensions of the shadow box.
			Vector3 minCorner(INFINITY, INFINITY, INFINITY), maxCorner(-INFINITY, -INFINITY, -INFINITY);
			for (unsigned int cc = 0; cc < activeCameraFrustum.CORNER_COUNT; cc++) {
				// X
				if (activeCameraFrustumCornersLightSpace[cc].x < minCorner.x) {
					minCorner.x = activeCameraFrustumCornersLightSpace[cc].x;
				}
				if (activeCameraFrustumCornersLightSpace[cc].x > maxCorner.x) {
					maxCorner.x = activeCameraFrustumCornersLightSpace[cc].x;
				}

				// Y
				if (activeCameraFrustumCornersLightSpace[cc].y < minCorner.y) {
					minCorner.y = activeCameraFrustumCornersLightSpace[cc].y;
				}
				if (activeCameraFrustumCornersLightSpace[cc].y > maxCorner.y) {
					maxCorner.y = activeCameraFrustumCornersLightSpace[cc].y;
				}

				// Z
				if (activeCameraFrustumCornersLightSpace[cc].z < minCorner.z) {
					minCorner.z = activeCameraFrustumCornersLightSpace[cc].z;
				}
				if (activeCameraFrustumCornersLightSpace[cc].z > maxCorner.z) {
					maxCorner.z = activeCameraFrustumCornersLightSpace[cc].z;
				}
			}

			double floor = 0.001;
			minCorner.x = (float)(minCorner.x - (float)modf(minCorner.x, &floor));
			minCorner.y = (float)(minCorner.y - (float)modf(minCorner.y, &floor));
			maxCorner.x = (float)(maxCorner.x - (float)modf(maxCorner.x, &floor));
			maxCorner.y = (float)(maxCorner.y - (float)modf(maxCorner.y, &floor));

			// Calculate shadow box dimensions.
			Vector3 shadowBoxDimensions;
			shadowBoxDimensions.x = maxCorner.x - minCorner.x;
			shadowBoxDimensions.y = maxCorner.y - minCorner.y;
			shadowBoxDimensions.z = maxCorner.z - minCorner.z;

			if (this->gShadowMap->isCascaded) {
				for (unsigned sf = 0; sf < this->gShadowMap->subFrustumCount; sf++) {
					// Calculate new position of the shadow map's camera.
					Vector3 newPosition;
					Vector3 shadowMapFrustumForward;
					shadowMapFrustumForward.storeXMVECTOR(
						&dx::XMVector3Transform(
							dx::XMVectorSet(0, 0, 1, 1),
							lightViewMatrix
						)
					);
					shadowMapFrustumForward.normalize();

					// Calculate center of active camera's frustum.
					Vector3 activeCameraFrustumCenter;
					Vector3 activeCameraFrustumForward;
					activeCameraFrustumForward.storeXMVECTOR(
						&dx::XMVector3Transform(
							dx::XMVectorSet(0, 0, 1, 1),
							activeCameraViewMatrix
						)
					);
					activeCameraFrustumForward.normalize();
					activeCameraFrustumCenter = activeCamera->gPosition + activeCameraFrustumForward * (((activeCameraFrustum.Far - activeCameraFrustum.Near) / 2) + activeCameraFrustum.Near);

					// Set shadow box's center position to center of the view frustum.
					this->gShadowMap->pCamera[sf]->setPosition(
						activeCameraFrustumCenter + (-shadowMapFrustumForward * shadowBoxDimensions.z / 2)
					);
					this->gShadowMap->pCamera[sf]->setDirection(direction);
					this->gShadowMap->pCamera[sf]->setOrthographicProjection(
						shadowBoxDimensions.x, shadowBoxDimensions.y,
						0.5, shadowBoxDimensions.z
					);
				}
			}
			else {
				// Calculate new position of the shadow map's camera.
				Vector3 newPosition;
				Vector3 shadowMapFrustumForward;
				shadowMapFrustumForward.storeXMVECTOR(
					&dx::XMVector3Transform(
						dx::XMVectorSet(0, 0, 1, 1),
						lightViewMatrix
					)
				);
				shadowMapFrustumForward.normalize();

				// Calculate center of active camera's frustum.
				Vector3 activeCameraFrustumCenter;
				Vector3 activeCameraFrustumForward;
				activeCameraFrustumForward.storeXMVECTOR(
					&dx::XMVector3Transform(
						dx::XMVectorSet(0, 0, 1, 1),
						activeCameraViewMatrix
					)
				);
				activeCameraFrustumForward.normalize();
				activeCameraFrustumCenter = activeCamera->gPosition + activeCameraFrustumForward * (((activeCameraFrustum.Far - activeCameraFrustum.Near) / 2) + activeCameraFrustum.Near);

				// Set shadow box's center position to center of the view frustum.
				this->gShadowMap->pCamera[0]->setPosition(
					activeCameraFrustumCenter + (-shadowMapFrustumForward * shadowBoxDimensions.z / 2)
				);
				this->gShadowMap->pCamera[0]->setDirection(direction);
				this->gShadowMap->pCamera[0]->setOrthographicProjection(
					shadowBoxDimensions.x, shadowBoxDimensions.y,
					0.5, shadowBoxDimensions.z
				);
			}
			break;
	}

	for (unsigned int sf = 0; sf < this->gShadowMap->subFrustumCount; sf++) {
		this->gShadowMap->pCamera[sf]->updateConstantBuffer();
	}
}