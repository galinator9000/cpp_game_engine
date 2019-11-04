#include "ShadowBox.h"

ShadowBox::ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType) {
	this->lightType = lightType;

	switch (this->lightType) {
		case LIGHT_TYPE::SPOT_LIGHT:
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, direction, WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			break;

		case LIGHT_TYPE::POINT_LIGHT:
			// One camera for each axes.
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, Vector3::XAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, Vector3::YAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, Vector3::ZAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, -Vector3::XAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, -Vector3::YAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(position, -Vector3::ZAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE),
					new RenderTarget()
				)
			);
			break;

		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			this->gShadowMaps.push_back(
				std::make_pair(
					new Camera(Vector3(0, 0, 0), direction, WIDTH, HEIGHT, PROJECTION_TYPE::ORTHOGRAPHIC),
					new RenderTarget()
				)
			);
			break;
	}
}

void ShadowBox::Update(Vector3 position, Vector3 direction) {
	for (unsigned int a = 0; a < this->gShadowMaps.size(); a++) {
		if (this->lightType == DIRECTIONAL_LIGHT || this->lightType == SPOT_LIGHT) {
			this->gShadowMaps.at(a).first->setDirection(direction);
		}
		if (this->lightType == POINT_LIGHT || this->lightType == SPOT_LIGHT) {
			this->gShadowMaps.at(a).first->setPosition(position);
		}
		this->gShadowMaps.at(a).first->updateConstantBuffer();
	}
}