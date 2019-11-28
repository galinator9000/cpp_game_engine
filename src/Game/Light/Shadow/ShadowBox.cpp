#include "ShadowBox.h"

ShadowBox::ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType) {
	this->lightType = lightType;

	Camera* shadowMapCamera;
	RenderTarget* shadowMapRenderTarget;
	Viewport* shadowMapViewport = new Viewport(
		{WIDTH, HEIGHT}
	);

	this->isActive = true;

	switch (this->lightType) {
		case LIGHT_TYPE::SPOT_LIGHT:
			shadowMapCamera = new Camera(position, direction, WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();

			this->gShadowMaps.push_back(
				new ShadowMap(
					shadowMapCamera,
					shadowMapRenderTarget,
					shadowMapViewport
				)
			);
			break;

		case LIGHT_TYPE::POINT_LIGHT:
			// One camera for each axes.
			// +X
			/*shadowBoxCamera = new Camera(position, Vector3::XAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// +Y
			shadowBoxCamera = new Camera(position, Vector3::YAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// +Z
			shadowBoxCamera = new Camera(position, Vector3::ZAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// -X
			shadowBoxCamera = new Camera(position, -Vector3::XAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// -Y
			shadowBoxCamera = new Camera(position, -Vector3::YAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// -Z
			shadowBoxCamera = new Camera(position, -Vector3::ZAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);*/
			break;

		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			shadowMapCamera = new Camera(Vector3(0, 0, 0), direction, WIDTH, HEIGHT, PROJECTION_TYPE::ORTHOGRAPHIC);
			shadowMapCamera->setOrthographicProjection(24, 24);
			shadowMapRenderTarget = new RenderTarget();

			this->gShadowMaps.push_back(
				new ShadowMap(
					shadowMapCamera,
					shadowMapRenderTarget,
					shadowMapViewport
				)
			);
			break;
	}
}

void ShadowBox::Update(Vector3 position, Vector3 direction, Camera* activeCamera) {
	for (unsigned int sm = 0; sm < this->gShadowMaps.size(); sm++) {
		switch (this->lightType) {
			case LIGHT_TYPE::SPOT_LIGHT:
				this->gShadowMaps.at(sm)->pCamera->setPosition(position);
				this->gShadowMaps.at(sm)->pCamera->setDirection(direction);
				break;

			case LIGHT_TYPE::POINT_LIGHT:
				this->gShadowMaps.at(sm)->pCamera->setPosition(position);
				this->gShadowMaps.at(sm)->pCamera->setDirection(direction);
				break;

			case LIGHT_TYPE::DIRECTIONAL_LIGHT:
				// Calculate active camera's frustum vertices.
				dx::BoundingFrustum activeCameraFrustum(
					dx::XMMatrixTranspose(
						dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.projectionMatrix)
					)
				);
				dx::XMMATRIX activeCameraViewMatrix = dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.viewMatrix);
				dx::XMMATRIX activeCameraViewMatrixInverse = dx::XMMatrixInverse(
					NULL,
					dx::XMMatrixTranspose(
						activeCameraViewMatrix
					)
				);
				activeCameraFrustum.Transform(activeCameraFrustum, activeCameraViewMatrixInverse);

				// Get corner positions.
				//dx::XMFLOAT3* activeCameraFrustumCorners = new dx::XMFLOAT3[activeCameraFrustum.CORNER_COUNT];
				//dx::XMFLOAT3* activeCameraFrustumCornersLightSpace = new dx::XMFLOAT3[activeCameraFrustum.CORNER_COUNT];

				dx::XMFLOAT3 activeCameraFrustumCorners[activeCameraFrustum.CORNER_COUNT];
				dx::XMFLOAT3 activeCameraFrustumCornersLightSpace[activeCameraFrustum.CORNER_COUNT];
				activeCameraFrustum.GetCorners(activeCameraFrustumCorners);

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

				// Calculate shadow box position and orientation with using active camera's frustum corner vertices.
				dx::XMMATRIX lightViewMatrix = dx::XMLoadFloat4x4(&this->gShadowMaps.at(sm)->pCamera->gCameraVSConstantBuffer.viewMatrix);

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

				// Calculate shadow box dimensions.
				Vector3 shadowBoxDimensions;
				shadowBoxDimensions.x = maxCorner.x - minCorner.x;
				shadowBoxDimensions.y = maxCorner.y - minCorner.y;
				shadowBoxDimensions.z = maxCorner.z - minCorner.z;

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

				// Set shadow box's center position to center of the view frustum.
				this->gShadowMaps.at(sm)->pCamera->setPosition(
					activeCameraFrustumCenter + (-shadowMapFrustumForward * shadowBoxDimensions.z / 2)
				);
				this->gShadowMaps.at(sm)->pCamera->setDirection(direction);
				this->gShadowMaps.at(sm)->pCamera->setOrthographicProjection(
					shadowBoxDimensions.x, shadowBoxDimensions.y,
					0.5, shadowBoxDimensions.z
				);

				break;
		}

		this->gShadowMaps.at(sm)->pCamera->updateConstantBuffer();
	}
}