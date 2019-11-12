#include "ShadowBox.h"

ShadowBox::ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType) {
	this->lightType = lightType;

	Camera* shadowMapCamera;
	RenderTarget* shadowMapRenderTarget;
	Viewport* shadowMapViewport = new Viewport(
		{WIDTH, HEIGHT}
	);

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

void ShadowBox::Update(Vector3 position, Vector3 direction) {
	for (unsigned int a = 0; a < this->gShadowMaps.size(); a++) {
		if (this->lightType == DIRECTIONAL_LIGHT || this->lightType == SPOT_LIGHT) {
			this->gShadowMaps.at(a)->pCamera->setDirection(direction);
		}
		if (this->lightType == POINT_LIGHT || this->lightType == SPOT_LIGHT) {
			this->gShadowMaps.at(a)->pCamera->setPosition(position);
		}
		this->gShadowMaps.at(a)->pCamera->updateConstantBuffer();
	}
}