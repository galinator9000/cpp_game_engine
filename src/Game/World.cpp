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
	for (unsigned int sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		this->gAllShadowBoxConstantBuffers[sb].isActive = false;

		for (unsigned int sm = 0; sm < MAX_SHADOWMAP_COUNT; sm++) {
			dx::XMStoreFloat4x4(
				&(this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].viewMatrix),
				dx::XMMatrixIdentity()
			);
			dx::XMStoreFloat4x4(
				&(this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].projectionMatrix),
				dx::XMMatrixIdentity()
			);
			this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].activeCameraSubfrustumNearPlaneDistance = 0;
			this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].activeCameraSubfrustumFarPlaneDistance = 0;
		}
	}
	this->pGfx->createShadowBoxesBuffer(
		&this->gAllShadowBoxConstantBuffers[0],
		MAX_SHADOWBOX_COUNT,
		&this->pAllShadowBoxConstantBuffers
	);
	this->pGfx->bindVertexShaderBuffer(3, this->pAllShadowBoxConstantBuffers.Get());
	this->pGfx->bindPixelShaderBuffer(3, this->pAllShadowBoxConstantBuffers.Get());

	// Set NULL all shadow casters initially.
	memset(
		&this->gShadowCasters,
		NULL,
		sizeof(Light*) * MAX_SHADOWBOX_COUNT
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
		sizeof(Light*) * MAX_SHADOWBOX_COUNT
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
			if (shadowCasterIndex < MAX_SHADOWBOX_COUNT) {
				float lightDist = 0;

				switch (light->type) {
					case LIGHT_TYPE::POINT_LIGHT:
						/*this->gShadowCasters[shadowCasterIndex] = light;
						shadowCasterIndex++; */
						break;

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
				}
			}
		}
	}

	// Fill shadow caster array, starting from nearest lights.
	if (shadowCasterIndex < MAX_SHADOWBOX_COUNT) {
		for (const auto entry : gShadowCastersDistanceLPMap) {
			this->gShadowCasters[shadowCasterIndex] = entry.second;
			shadowCasterIndex++;
			if (shadowCasterIndex >= MAX_SHADOWBOX_COUNT) {
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
	for (unsigned int sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		if (this->gShadowCasters[sb] == NULL) {
			continue;
		}

		// Copy position & direction values from light to camera.
		switch (this->gShadowCasters[sb]->gShadowBox->lightType) {
			case DIRECTIONAL_LIGHT:
				this->gShadowCasters[sb]->gShadowBox->Update(
					this->gShadowCasters[sb]->gPosition,
					this->gShadowCasters[sb]->gDirection.normalize(),
					this->activeCamera
				);
				break;
			case POINT_LIGHT:
				this->gShadowCasters[sb]->gShadowBox->Update(
					this->gShadowCasters[sb]->gPosition,
					this->gShadowCasters[sb]->gDirection.normalize(),
					this->activeCamera
				);
				break;
			case SPOT_LIGHT:
				this->gShadowCasters[sb]->gShadowBox->Update(
					this->gShadowCasters[sb]->gPosition,
					this->gShadowCasters[sb]->gDirection.normalize(),
					this->activeCamera
				);
				break;
		}

		for (unsigned int sm = 0; sm < this->gShadowCasters[sb]->gShadowBox->gShadowMapCount; sm++) {
			// Set render target and camera for rendering scene from light's "view".
			this->pGfx->setViewport(this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pViewport);
			this->pGfx->setRenderTarget(this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pRenderTarget);
			this->pGfx->updateCamera(this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pCamera);
			this->pGfx->activateCamera(this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pCamera);

			// Render entity.
			for (unsigned int e = 0; e < this->allEntities.size(); e++) {
				Entity* ent = this->allEntities.at(e);

				if (ent == NULL) {
					continue;
				}

				this->pGfx->drawEntity(ent);
			}
		}
	}

	//// Draw all entities.
	// Set scene camera back.
	this->pGfx->activateCamera(this->activeCamera);

	// Set main shaders & render target.
	this->pGfx->setVertexShader(this->pGfx->mainVertexShader);
	this->pGfx->setPixelShader(this->pGfx->mainPixelShader);
	this->pGfx->setViewport(this->pGfx->mainViewport);
	this->pGfx->setRenderTarget(this->pGfx->mainRenderTarget);

	// Provide View & Projection matrices of shadow boxes to Vertex & Pixel Shader.
	for (unsigned int sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		if (this->gShadowCasters[sb] == NULL) {
			this->gAllShadowBoxConstantBuffers[sb].isActive = false;

			for (unsigned int sm = 0; sm < MAX_SHADOWMAP_COUNT; sm++) {
				dx::XMStoreFloat4x4(&this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].viewMatrix, dx::XMMatrixIdentity());
				dx::XMStoreFloat4x4(&this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].projectionMatrix, dx::XMMatrixIdentity());
				this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].activeCameraSubfrustumNearPlaneDistance = 0;
				this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].activeCameraSubfrustumFarPlaneDistance = 0;
			}
		}
		else {
			this->gAllShadowBoxConstantBuffers[sb].isActive = this->gShadowCasters[sb]->gShadowBox->isActive;
			this->gAllShadowBoxConstantBuffers[sb].lightType = this->gShadowCasters[sb]->gShadowBox->lightType;
			this->gAllShadowBoxConstantBuffers[sb].lightID = this->gShadowCasters[sb]->id;
			this->gAllShadowBoxConstantBuffers[sb].shadowDistance = this->gShadowCasters[sb]->gShadowBox->getShadowDistance();
			this->gAllShadowBoxConstantBuffers[sb].shadowMapCount = this->gShadowCasters[sb]->gShadowBox->gShadowMapCount;

			for (unsigned int sm = 0; sm < this->gShadowCasters[sb]->gShadowBox->gShadowMapCount; sm++) {
				this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].viewMatrix = this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pCamera->gCameraVSConstantBuffer.viewMatrix;
				this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].projectionMatrix = this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pCamera->gCameraVSConstantBuffer.projectionMatrix;
				this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].activeCameraSubfrustumNearPlaneDistance = this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->activeCameraSubfrustumNearPlaneDistance;
				this->gAllShadowBoxConstantBuffers[sb].shadowMap[sm].activeCameraSubfrustumFarPlaneDistance = this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->activeCameraSubfrustumFarPlaneDistance;
			}
		}
	}
	this->pGfx->updateShadowBoxesBuffer(&this->gAllShadowBoxConstantBuffers[0], MAX_SHADOWBOX_COUNT, this->pAllShadowBoxConstantBuffers.Get());
	this->pGfx->bindVertexShaderBuffer(3, this->pAllShadowBoxConstantBuffers.Get());
	this->pGfx->bindPixelShaderBuffer(3, this->pAllShadowBoxConstantBuffers.Get());

	// Provide shadow mappings to Pixel Shader
	for (unsigned int sb = 0; sb < MAX_SHADOWBOX_COUNT; sb++) {
		if (this->gShadowCasters[sb] == NULL) {
			continue;
		}

		for (unsigned int sm = 0; sm < this->gShadowCasters[sb]->gShadowBox->gShadowMapCount; sm++) {
			this->pGfx->setTexturePixelShader(SHADOWMAP_TEXTURE_PS_START_SLOT + (sb * MAX_SHADOWMAP_COUNT + sm), this->gShadowCasters[sb]->gShadowBox->gShadowMaps[sm]->pTexture);
		}
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
		for (unsigned int sm = 0; sm < light->gShadowBox->gShadowMapCount; sm++) {
			this->pGfx->addCamera(light->gShadowBox->gShadowMaps[sm]->pCamera, false);
			this->pGfx->createShadowMapTexture(light->gShadowBox->gShadowMaps[sm]->pTexture);
			this->pGfx->createRenderTarget(
				light->gShadowBox->gShadowMaps[sm]->pRenderTarget,
				NULL,
				false,
				true,
				light->gShadowBox->gShadowMaps[sm]->pTexture
			);
			this->pGfx->renderTargets.push_back(light->gShadowBox->gShadowMaps[sm]->pRenderTarget);
		}
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