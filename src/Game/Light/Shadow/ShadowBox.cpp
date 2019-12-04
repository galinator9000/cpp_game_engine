#include "ShadowBox.h"

ShadowBox::ShadowBox(Vector3 position, Vector3 direction, Vector2 mapDimensions, LIGHT_TYPE lightType, unsigned int shadowMapCount) {
	this->lightType = lightType;

	if (shadowMapCount > MAX_SHADOWMAP_COUNT) {
		shadowMapCount = MAX_SHADOWMAP_COUNT;
	}
	switch (this->lightType) {
		// Cascaded Shadow Mapping.
		// One shadow map for each subfrustum
		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			this->gShadowMapCount = shadowMapCount;
			break;
		// One shadow map for each XYZ and -XYZ axes
		case LIGHT_TYPE::POINT_LIGHT:
			shadowMapCount = 6;
			this->gShadowMapCount = shadowMapCount;
			break;
		case LIGHT_TYPE::SPOT_LIGHT:
			shadowMapCount = 1;
			this->gShadowMapCount = shadowMapCount;
			break;
	}

	// Create array for holding shadow maps.
	this->gShadowMaps = new ShadowMap*[this->gShadowMapCount];

	switch (this->lightType) {
		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			for (unsigned int sm = 0; sm < this->gShadowMapCount; sm++) {
				this->gShadowMaps[sm] = new ShadowMap(
					Vector3(0, 0, 0), direction, mapDimensions, PROJECTION_TYPE::ORTHOGRAPHIC
				);
			}
			break;
		case LIGHT_TYPE::POINT_LIGHT:
			for (unsigned int sm = 0; sm < this->gShadowMapCount; sm++) {
				this->gShadowMaps[sm] = new ShadowMap(
					position, Vector3(0, 0, 1), mapDimensions, PROJECTION_TYPE::PERSPECTIVE
				);
			}
			break;
		case LIGHT_TYPE::SPOT_LIGHT:
			for (unsigned int sm = 0; sm < this->gShadowMapCount; sm++) {
				this->gShadowMaps[sm] = new ShadowMap(
					position, direction, mapDimensions, PROJECTION_TYPE::PERSPECTIVE
				);
			}
			break;
	}
	this->isActive = true;
}

void ShadowBox::Update(Vector3 lightPosition, Vector3 lightDirection, Camera* activeCamera) {
	switch (this->lightType) {
		case LIGHT_TYPE::SPOT_LIGHT:
			this->gShadowMaps[0]->pCamera->setPosition(lightPosition);
			this->gShadowMaps[0]->pCamera->setDirection(lightDirection);
			this->gShadowMaps[0]->pCamera->updateConstantBuffer();
			break;

		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			// Calculate active camera's frustum vertices.
			dx::BoundingFrustum activeCameraFrustum(
				dx::XMMatrixTranspose(
					dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.projectionMatrix)
				)
			);

			// Adjust far plane distance.
			this->gShadowDistance = (float) abs(activeCameraFrustum.Far - activeCameraFrustum.Near) * this->gShadowDistanceRatio;
			activeCameraFrustum.Far = this->gShadowDistance;

			dx::XMMATRIX activeCameraViewMatrix = dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.viewMatrix);
			dx::XMMATRIX activeCameraViewMatrixInverse = dx::XMMatrixInverse(
				NULL,
				dx::XMMatrixTranspose(
					activeCameraViewMatrix
				)
			);

			// Get corner positions of each frustum.
			float subFrustumZStep = (activeCameraFrustum.Far - activeCameraFrustum.Near) / this->gShadowMapCount;

			dx::BoundingFrustum* subFrustum;
			dx::BoundingFrustum* subFrustumLightSpace;
			dx::XMFLOAT3 subFrustumCornersLightSpace[activeCameraFrustum.CORNER_COUNT];
			for (unsigned int sm = 0; sm < this->gShadowMapCount; sm++) {
				// Copy active camera's view frustum, adjust far and near planes and finally calculate subfrustum's corners.
				subFrustum = new dx::BoundingFrustum(activeCameraFrustum);
				subFrustum->Near = activeCameraFrustum.Near + subFrustumZStep * sm;
				subFrustum->Far = activeCameraFrustum.Near + subFrustumZStep * (sm + 1);

				// Transform corner positions to world space.
				subFrustum->Transform(*subFrustum, activeCameraViewMatrixInverse);

				// Transform subfrustum vertices from world space to light space.
				subFrustumLightSpace = new dx::BoundingFrustum(*subFrustum);
				subFrustumLightSpace->Transform(
					*subFrustumLightSpace,
					dx::XMMatrixTranspose(
						dx::XMMatrixLookAtLH(
							lightPosition.loadXMVECTOR(),
							(lightPosition + lightDirection).loadXMVECTOR(),
							dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
						)
					)
				);

				// Get corners of the subfrustum.
				subFrustumLightSpace->GetCorners(subFrustumCornersLightSpace);

				// Calculate dimensions of the current subfrustum's shadow box.
				Vector3 minCorner(INFINITY, INFINITY, INFINITY), maxCorner(-INFINITY, -INFINITY, -INFINITY);
				for (unsigned int cc = 0; cc < activeCameraFrustum.CORNER_COUNT; cc++) {
					// X
					minCorner.x = min(minCorner.x, subFrustumCornersLightSpace[cc].x);
					maxCorner.x = max(maxCorner.x, subFrustumCornersLightSpace[cc].x);

					// Y
					minCorner.y = min(minCorner.y, subFrustumCornersLightSpace[cc].y);
					maxCorner.y = max(maxCorner.y, subFrustumCornersLightSpace[cc].y);

					// Z
					minCorner.z = min(minCorner.z, subFrustumCornersLightSpace[cc].z);
					maxCorner.z = max(maxCorner.z, subFrustumCornersLightSpace[cc].z);
				}

				double floor = 0.001;
				minCorner.x = (float)(minCorner.x - (float) modf(minCorner.x, &floor));
				minCorner.y = (float)(minCorner.y - (float) modf(minCorner.y, &floor));
				minCorner.z = (float)(minCorner.z - (float) modf(minCorner.z, &floor));
				maxCorner.x = (float)(maxCorner.x - (float) modf(maxCorner.x, &floor));
				maxCorner.y = (float)(maxCorner.y - (float) modf(maxCorner.y, &floor));
				maxCorner.z = (float)(maxCorner.z - (float) modf(maxCorner.z, &floor));

				// Calculate shadow box dimensions.
				Vector3 shadowBoxDimensions(
					abs(maxCorner.x - minCorner.x),
					abs(maxCorner.y - minCorner.y),
					abs(maxCorner.z - minCorner.z)
				);

				// Calculate center of subfrustum.
				Vector3 subFrustumCenter;
				subFrustumCenter = activeCamera->gPosition + (activeCamera->gDirection.normalize() * (((subFrustum->Far - subFrustum->Near) / 2) + subFrustum->Near));

				// Set shadow box's position to center of the subfrustum.
				this->gShadowMaps[sm]->pCamera->setDirection(lightDirection);
				this->gShadowMaps[sm]->pCamera->setPosition(
					subFrustumCenter + (-lightDirection * shadowBoxDimensions.z / 2)
				);

				// Recalculate orthographic projection matrices on subfrustum cameras.
				this->gShadowMaps[sm]->pCamera->setOrthographicProjection(
					shadowBoxDimensions.x, shadowBoxDimensions.y,
					0.5f,
					shadowBoxDimensions.z
				);

				// Update constant buffer & frustum information.
				this->gShadowMaps[sm]->pCamera->updateConstantBuffer();

				this->gShadowMaps[sm]->activeCameraSubfrustumNearPlaneDistance = subFrustum->Near;
				this->gShadowMaps[sm]->activeCameraSubfrustumFarPlaneDistance = subFrustum->Far;
			}

			break;
	}
}