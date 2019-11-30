#include "World.h"

World::World(Graphics* pGfx, Physics* pPhy, Audio* pAud){
	this->pGfx = pGfx;
	this->pPhy = pPhy;
	this->pAud = pAud;

	this->Setup();
}

void World::Setup() {
	// Create buffer that will hold lights.
	// Set undefined lights' intensity to -1.
	for (unsigned int light = 0; light<MAX_LIGHT_COUNT; light++) {
		gAllLightConstantBuffers[light].intensity = -1.0f;

		gAllLightConstantBuffers[light].color = dx::XMFLOAT4(0, 0, 0, 0);
		gAllLightConstantBuffers[light].direction = dx::XMFLOAT3(0, 0, 0);
		gAllLightConstantBuffers[light].position = dx::XMFLOAT3(0, 0, 0);
		gAllLightConstantBuffers[light].halfSpotAngle = 0;
	}
	this->pGfx->createLightsBuffer(
		&this->gAllLightConstantBuffers[0],
		MAX_LIGHT_COUNT,
		&this->pAllLightConstantBuffers
	);
	this->pGfx->bindPixelShaderBuffer(1, this->pAllLightConstantBuffers.Get());

	// Create buffer that will hold shadow casters.
	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		dx::XMStoreFloat4x4(
			&(gAllShadowMapConstantBuffers[sc].viewMatrix),
			dx::XMMatrixIdentity()
		);

		dx::XMStoreFloat4x4(
			&(gAllShadowMapConstantBuffers[sc].projectionMatrix),
			dx::XMMatrixIdentity()
		);
		gAllShadowMapConstantBuffers[sc].isActive = false;
	}
	this->pGfx->createShadowMapsBuffer(
		&this->gAllShadowMapConstantBuffers[0],
		MAX_SHADOW_CASTER_COUNT,
		&this->pAllShadowMapConstantBuffers
	);
	this->pGfx->bindVertexShaderBuffer(3, this->pAllShadowMapConstantBuffers.Get());

	// Set NULL all shadow casters initially.
	memset(
		&this->gShadowCasters,
		NULL,
		sizeof(Light*) * MAX_SHADOW_CASTER_COUNT
	);
}

void World::Reset() {

}

void World::Update(){
	//// Update section.
	// Step and update physics of the world.
	this->pPhy->Update();

	// Update vector relations.
	for (unsigned int vr = 0; vr < allVectorRelations.size(); vr++) {
		allVectorRelations.at(vr)->Update();
	}

	// Update all light objects.
	// Also pick lights that will cast shadow.
	// (directional lights has priority, nearest to active camera)
	unsigned int shadowCasterIndex = 0;
	memset(
		&this->gShadowCasters,
		NULL,
		sizeof(Light*) * MAX_SHADOW_CASTER_COUNT
	);
	gShadowCastersDistanceLPMap.clear();

	bool shouldUpdateLightsGPUData = false;
	for (unsigned int l = 0; l < allLights.size(); l++) {
		Light* light = allLights.at(l);

		if (light == NULL) {
			continue;
		}

		light->Update();

		if (light->shouldUpdateGPUData) {
			// Update data for each light.
			this->gAllLightConstantBuffers[light->id].type = light->type;
			this->gAllLightConstantBuffers[light->id].isActive = light->isActive;
			this->gAllLightConstantBuffers[light->id].intensity = light->gIntensity;
			this->gAllLightConstantBuffers[light->id].halfSpotAngle = light->gHalfSpotAngle;
			this->gAllLightConstantBuffers[light->id].color = light->gColor.loadXMFLOAT();
			this->gAllLightConstantBuffers[light->id].position = light->gPosition.loadXMFLOAT();
			this->gAllLightConstantBuffers[light->id].direction = light->gDirection.loadXMFLOAT();

			light->shouldUpdateGPUData = false;
			shouldUpdateLightsGPUData = true;
		}

		if (light->isCastingShadow) {
			if (shadowCasterIndex < MAX_SHADOW_CASTER_COUNT) {
				float lightDist = 0;

				switch (light->type) {
					case LIGHT_TYPE::DIRECTIONAL_LIGHT:
						this->gShadowCasters[shadowCasterIndex] = light;
						shadowCasterIndex++;
						break;

					case LIGHT_TYPE::SPOT_LIGHT:
						lightDist = Vector3::distance(
							light->gPosition,
							this->activeCamera->gPosition
						);
						gShadowCastersDistanceLPMap[lightDist] = light;
						break;

					case LIGHT_TYPE::POINT_LIGHT:
						lightDist = Vector3::distance(
							light->gPosition,
							this->activeCamera->gPosition
						);
						gShadowCastersDistanceLPMap[lightDist] = light;
						break;
				}
			}
		}
	}

	// Fill shadow caster array, starting from nearest lights.
	if (shadowCasterIndex < MAX_SHADOW_CASTER_COUNT) {
		for (const auto entry : gShadowCastersDistanceLPMap) {
			this->gShadowCasters[shadowCasterIndex] = entry.second;
			shadowCasterIndex++;
			if (shadowCasterIndex >= MAX_SHADOW_CASTER_COUNT) {
				break;
			}
		}
	}
	
	// Update lights buffer on GPU side.
	if (shouldUpdateLightsGPUData) {
		this->pGfx->updateLightsBuffer(
			&this->gAllLightConstantBuffers[0],
			MAX_LIGHT_COUNT,
			this->pAllLightConstantBuffers.Get()
		);
	}

	// Update all cameras.
	for (unsigned int c = 0; c < this->allCameras.size(); c++) {
		Camera* cam = allCameras.at(c);

		if (cam == NULL) {
			continue;
		}

		cam->Update();
		this->pPhy->updateCamera(cam);
	}
	// Update GPU data of active camera.
	this->pGfx->updateCamera(this->activeCamera);

	// Update all entities.
	for (unsigned int e = 0; e < this->allEntities.size(); e++) {
		Entity* ent = this->allEntities.at(e);

		if (ent == NULL) {
			continue;
		}

		this->pPhy->updateEntity(ent);
		ent->Update();
		this->pGfx->updateEntity(ent);
		ent->Reset();
	}

	// Render current state of the world.
	this->Render();

	//// Reset section
	for (unsigned int l = 0; l < allLights.size(); l++) {
		Light* light = allLights.at(l);

		if (light == NULL) {
			continue;
		}

		light->Reset();
	}
}

void World::Render() {
	// Clear frame and redraw state of the world.
	this->pGfx->beginFrame();
	
	//// Create shadow map from active shadow caster.
	// Set depth-only rendering shaders first.
	this->pGfx->setVertexShader(this->pGfx->depthVertexShader);
	this->pGfx->setPixelShader(this->pGfx->depthPixelShader);

	// Render only depth values of entities for each shadow map.
	Camera* shadowMapCamera;
	RenderTarget* shadowMapRenderTarget;
	Viewport* shadowMapViewport;

	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		if (this->gShadowCasters[sc] == NULL) {
			continue;
		}

		// Copy position & direction values from light to camera.
		switch (this->gShadowCasters[sc]->gShadowBox->lightType) {
			case DIRECTIONAL_LIGHT:
				this->gShadowCasters[sc]->gShadowBox->Update(
					this->gShadowCasters[sc]->gPosition,
					this->gShadowCasters[sc]->gDirection.normalize(),
					this->activeCamera
				);
				break;
			case SPOT_LIGHT:
				this->gShadowCasters[sc]->gShadowBox->Update(
					this->gShadowCasters[sc]->gPosition,
					this->gShadowCasters[sc]->gDirection.normalize(),
					this->activeCamera
				);
				break;
				this->gShadowCasters[sc]->gShadowBox->Update(
					this->gShadowCasters[sc]->gPosition,
					this->gShadowCasters[sc]->gDirection,
					this->activeCamera
				);
			case POINT_LIGHT:
				break;
		}

		shadowMapCamera = this->gShadowCasters[sc]->gShadowBox->gShadowMap->pCamera;
		shadowMapRenderTarget = this->gShadowCasters[sc]->gShadowBox->gShadowMap->pRenderTarget;
		shadowMapViewport = this->gShadowCasters[sc]->gShadowBox->gShadowMap->pViewPort;

		// Set render target and camera for rendering scene from light's "view".
		this->pGfx->setViewport(shadowMapViewport);
		this->pGfx->setRenderTarget(shadowMapRenderTarget);
		this->pGfx->updateCamera(shadowMapCamera);
		this->pGfx->activateCamera(shadowMapCamera);

		// Render entity.
		for (unsigned int e = 0; e < this->allEntities.size(); e++) {
			Entity* ent = this->allEntities.at(e);

			if (ent == NULL) {
				continue;
			}

			this->pGfx->drawEntity(ent);
		}
	}

	//// Draw all entities.
	// Set scene camera back.
	this->pGfx->activateCamera(this->activeCamera);

	// Set main shaders & render target.
	this->pGfx->setVertexShader(this->pGfx->mainVertexShader);
	this->pGfx->setPixelShader(this->pGfx->mainPixelShader);
	this->pGfx->setRenderTarget(this->pGfx->mainRenderTarget);

	// Provide View & Projection matrices of shadow boxes to Vertex & Pixel Shader.
	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		if (this->gShadowCasters[sc] == NULL) {
			dx::XMStoreFloat4x4(&this->gAllShadowMapConstantBuffers[sc].viewMatrix, dx::XMMatrixIdentity());
			dx::XMStoreFloat4x4(&this->gAllShadowMapConstantBuffers[sc].projectionMatrix, dx::XMMatrixIdentity());
			this->gAllShadowMapConstantBuffers[sc].isActive = false;
		}
		else {
			this->gAllShadowMapConstantBuffers[sc].viewMatrix = this->gShadowCasters[sc]->gShadowBox->gShadowMap->pCamera->gCameraVSConstantBuffer.viewMatrix;
			this->gAllShadowMapConstantBuffers[sc].projectionMatrix = this->gShadowCasters[sc]->gShadowBox->gShadowMap->pCamera->gCameraVSConstantBuffer.projectionMatrix;
			this->gAllShadowMapConstantBuffers[sc].isActive = this->gShadowCasters[sc]->gShadowBox->isActive;
			this->gAllShadowMapConstantBuffers[sc].shadowDistance = this->gShadowCasters[sc]->gShadowBox->getShadowDistance();
			this->gAllShadowMapConstantBuffers[sc].lightType = this->gShadowCasters[sc]->gShadowBox->lightType;
			this->gAllShadowMapConstantBuffers[sc].lightID = this->gShadowCasters[sc]->id;
		}
	}
	this->pGfx->updateShadowMapsBuffer(&this->gAllShadowMapConstantBuffers[0], MAX_SHADOW_CASTER_COUNT, this->pAllShadowMapConstantBuffers.Get());
	this->pGfx->bindVertexShaderBuffer(3, this->pAllShadowMapConstantBuffers.Get());
	this->pGfx->bindPixelShaderBuffer(2, this->pAllShadowMapConstantBuffers.Get());

	// Provide shadow mappings to Pixel Shader
	const unsigned int SHADOW_MAP_TEXTURE_PS_START_SLOT = 3;
	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		if (this->gShadowCasters[sc] == NULL) {
			continue;
		}

		this->pGfx->setTexturePixelShader(SHADOW_MAP_TEXTURE_PS_START_SLOT + sc, this->gShadowCasters[sc]->gShadowBox->gShadowMap->pRenderTarget->pTexture);
	}
	
	for (unsigned int e = 0; e < this->allEntities.size(); e++) {
		Entity* ent = this->allEntities.at(e);

		if (ent == NULL) {
			continue;
		}

		this->pGfx->drawEntity(ent);
	}

	this->pGfx->endFrame();
}

// Entity
bool World::addEntity(Entity* entity){
	bool pAddResult = this->pPhy->addEntity(entity);
	bool gAddResult = this->pGfx->addEntity(entity);

	if (!pAddResult || !gAddResult) {
		return false;
	}

	entity->id = (unsigned int) this->allEntities.size();
	this->allEntities.push_back(entity);

	return true;
}

// Light
bool World::addLight(Light* light) {
	if (this->allLights.size() >= MAX_LIGHT_COUNT) {
		return false;
	}

	// Create render target for shadow maps.
	if (light->isCastingShadow && light->gShadowBox != NULL) {
		Camera* shadowMapCamera = light->gShadowBox->gShadowMap->pCamera;
		RenderTarget* shadowMapRenderTarget = light->gShadowBox->gShadowMap->pRenderTarget;

		this->pGfx->addCamera(shadowMapCamera, false);
		this->pGfx->createRenderTarget(shadowMapRenderTarget, true);
		this->pGfx->renderTargets.push_back(shadowMapRenderTarget);
	}

	light->id = (unsigned int) this->allLights.size();
	this->allLights.push_back(light);

	return true;
}

// Camera
bool World::addCamera(Camera* camera, bool setAsMain){
	setAsMain = (setAsMain || this->activeCamera == NULL);
	bool gAddResult = this->pGfx->addCamera(camera, setAsMain);
	bool pAddResult = this->pPhy->addCamera(camera);

	if (gAddResult || pAddResult) {
		camera->id = (unsigned int)this->allCameras.size();
		this->allCameras.push_back(camera);

		if (setAsMain) {
			this->setCamera(camera);
		}

		return true;
	}
	return false;
}

bool World::setCamera(Camera* camera) {
	if (std::find(this->allCameras.begin(), this->allCameras.end(), camera) != this->allCameras.end()) {
		this->activeCamera = camera;
		this->pGfx->activateCamera(this->activeCamera);
		return true;
	}
	else {
		return false;
	}
}

void World::switchCamera() {
	// If there's only one camera, simply return.
	if (this->allCameras.size() <= 1) {
		return;
	}

	unsigned int nextCamID = this->activeCamera->id;
	while(nextCamID <= (unsigned int) this->allCameras.size()-1){
		nextCamID++;
		if (nextCamID >= this->allCameras.size()) {
			nextCamID = 0;
		}

		if (this->allCameras.at(nextCamID) != NULL) {
			break;
		}
	}

	this->setCamera(this->allCameras.at(nextCamID));
}

// Texture
void World::createTexture(Texture* texture) {
	this->allTextures.push_back(texture);
	this->pGfx->createTextureDDS(texture);
}

// Vector Relation
void World::addVectorRelation(VectorRelation* vectorRelation) {
	vectorRelation->id = (unsigned int) this->allVectorRelations.size();
	this->allVectorRelations.push_back(vectorRelation);
}

// Physics
bool World::createFixedJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
){
	return this->pPhy->createFixedJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}

bool World::createDistanceJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
){
	return this->pPhy->createDistanceJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}

bool World::createSphericalJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
){
	return this->pPhy->createSphericalJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}

bool World::createRevoluteJoint(
	Entity* entity1, Entity* entity2,
	Vector3 transform1, Vector3 transform2
) {
	return this->pPhy->createRevoluteJoint(
		entity1->pCollisionActor, entity2->pCollisionActor,
		transform1.toPxTransform(),
		transform2.toPxTransform()
	);
}