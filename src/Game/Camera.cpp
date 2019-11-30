#include "Camera.h"

Camera::Camera(Vector3 position, Vector3 direction, float WIDTH, float HEIGHT, PROJECTION_TYPE projectionType){
	this->gPosition = position;
	this->gDirection = direction.normalize();

	this->rotation = Vector3();
	this->gDirection.storeXMVECTOR(
		&dx::XMVector3Transform(
			dx::XMVectorSet(0, 0, 1, 0),
			dx::XMMatrixRotationRollPitchYaw(
				this->rotation.x,
				this->rotation.y,
				0
			)
		)
	);
	this->camLookAt = position + direction;

	// Default perspective.
	switch (projectionType) {
		case PERSPECTIVE:
			this->setPerspectiveProjection();
			break;
		case ORTHOGRAPHIC:
			this->setOrthographicProjection(WIDTH, HEIGHT);
			break;
	}

	// Create physics shape&actor.
	this->pCollisionActor = new CollisionActor(COLLISION_ACTOR_KINEMATIC);
	this->pCollisionShape = new CollisionShape();
	this->pCollisionShape->createBoxGeometry({ 0.1f, 0.1f, 0.1f });

	this->updateConstantBuffer();
}

void Camera::setPosition(Vector3 newPos) {
	this->gPosition = newPos;

	// Update physics actor's position.
	if (this->pCollisionActor->pActor != NULL) {
		PxRigidDynamic* rigidDynamic = this->pCollisionActor->pActor->is<PxRigidDynamic>();
		if (rigidDynamic != NULL) {
			rigidDynamic->setGlobalPose(this->gPosition.toPxTransform());
		}
	}

	this->dataChanged = true;
}

void Camera::setDirection(Vector3 newDir) {
	this->gDirection = newDir;
	this->dataChanged = true;
}

void Camera::Update() {
	this->shouldUpdateGPUData = false;

	if (this->isFollowingEntity || this->dataChanged) {
		this->dataChanged = false;

		if (this->isFollowingEntity) {
			// Set camera's position to (entity's position + (follow offset * rotation)).
			this->gPosition.storeXMVECTOR(
				&dx::XMVector3Transform(
				(this->entityCenterOffset + this->followEntityOffset).loadXMVECTOR(),
					(
						// Rotate offset by rotation values.
						dx::XMMatrixRotationRollPitchYaw(
							this->rotation.x,
							this->rotation.y,
							0
						) *
						// Translate calculated position to world-space.
						dx::XMMatrixTranslation(
							this->followedEntity->gPosition.x,
							this->followedEntity->gPosition.y,
							this->followedEntity->gPosition.z
						)
					)
				)
			);

			// Update physics actor's position.
			if (this->pCollisionActor->pActor != NULL) {
				PxRigidDynamic* rigidDynamic = this->pCollisionActor->pActor->is<PxRigidDynamic>();
				if (rigidDynamic != NULL) {
					rigidDynamic->setGlobalPose(this->gPosition.toPxTransform());
				}
			}
		}
		else {
			// Update looking direction vector
			this->gDirection.storeXMVECTOR(
				&dx::XMVector3Transform(
					dx::XMVectorSet(0, 0, 1, 0),
					dx::XMMatrixRotationRollPitchYaw(
						this->rotation.x,
						this->rotation.y,
						0
					)
				)
			);
		}

		this->updateConstantBuffer();

		this->shouldUpdateGPUData = true;
	}
}

void Camera::updateConstantBuffer() {
	if (this->isFollowingEntity) {
		// Set looking direction to entity's position.
		this->camLookAt = this->followedEntity->gPosition + this->entityCenterOffset;
	}
	else {
		this->camLookAt = this->gPosition + this->gDirection;
	}

	//// Update constant buffer on class which is provided to GPU side.
	// Update View matrix.
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.viewMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixLookAtLH(
				this->gPosition.loadXMVECTOR(),
				this->camLookAt.loadXMVECTOR(),
				dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
			)
		)
	);

	// Provide position of the camera to buffer.
	this->gCameraVSConstantBuffer.cameraPosition = this->gPosition.loadXMFLOAT();
}

void Camera::Move(Vector3 translation, bool moveFast) {
	// Cameras that followes an entity, shouldn't be translated by hand.
	if (this->isFollowingEntity) {
		return;
	}

	if (moveFast && this->wasMovingFast) {
		this->currentMovementSpeed *= fastMovementFactor;
	}
	else {
		this->currentMovementSpeed = this->initialMovementSpeed;
	}

	// Calculate camera movement direction.
	dx::XMFLOAT3 camTranslation = dx::XMFLOAT3(translation.x, translation.y, translation.z);

	dx::XMStoreFloat3(
		&camTranslation,
		dx::XMVector3Transform(
			dx::XMLoadFloat3(&camTranslation),
			(
				dx::XMMatrixRotationRollPitchYaw(
					this->rotation.x,
					this->rotation.y,
					0
				) *
				dx::XMMatrixScaling(currentMovementSpeed, currentMovementSpeed, currentMovementSpeed)
			)
		)
	);

	// Update camera position
	this->gPosition.x += camTranslation.x;
	this->gPosition.y += camTranslation.y;
	this->gPosition.z += camTranslation.z;

	// Update physics actor's position.
	if (this->pCollisionActor->pActor != NULL) {
		PxRigidDynamic* rigidDynamic = this->pCollisionActor->pActor->is<PxRigidDynamic>();
		if (rigidDynamic != NULL) {
			rigidDynamic->setGlobalPose(this->gPosition.toPxTransform());
		}
	}

	// Update looking direction vector
	this->camLookAt = this->gPosition + this->gDirection;

	this->wasMovingFast = moveFast;
	this->dataChanged = true;
}

void Camera::Rotate(float xDelta, float yDelta) {
	xDelta *= this->currentRotationSpeed;
	yDelta *= this->currentRotationSpeed;

	// Update camera rotation
	// xDelta and yDelta comes from mouse's raw movement.
	// Mouse movement on +X axis (on screen space) corresponds to rotation on Y axis on our 3D world.
	if (this->isFollowingEntity) {
		// Rotation is inverted on X axis while following entity.
		this->rotation.x -= yDelta;
		this->rotation.y += xDelta;

		// Clip Pitch (Rotation on X axis) value.
		if (this->rotation.x >= (dx::XM_PI / 6)) {
			this->rotation.x = (dx::XM_PI * 0.1665f);
		}
		else if (this->rotation.x <= -(dx::XM_PI / 4)) {
			this->rotation.x = -(dx::XM_PI * 0.2499f);
		}
	}
	else {
		this->rotation.x += yDelta;
		this->rotation.y += xDelta;

		// Clip Pitch (Rotation on X axis) value to
		// -PI/2 and PI/2 in radians
		// -90 and 90 in degrees
		if (this->rotation.x >= dx::XM_PI / 2) {
			this->rotation.x = dx::XM_PI * 0.4999f;
		}
		else if (this->rotation.x <= -(dx::XM_PI / 2)) {
			this->rotation.x = -(dx::XM_PI * 0.4999f);
		}
	}

	this->dataChanged = true;
}

// Entity following functionality.
void Camera::followEntity(Entity* followEntity, Vector3 entityCenterOffset, Vector3 followEntityOffset) {
	this->followedEntity = followEntity;
	this->entityCenterOffset = entityCenterOffset;
	this->followEntityOffset = followEntityOffset;

	// Set values to initial values.
	float currentMovementSpeed = initialMovementSpeed;
	float currentRotationSpeed = initialRotationSpeed;
	this->rotation = Vector3();

	this->isFollowingEntity = true;
}

void Camera::Zoom(int zoomDirection) {
	if (this->isFollowingEntity){
		// Calculate direction of the offset.
		dx::XMFLOAT3 followEntityOffsetDirection;
		dx::XMStoreFloat3(
			&followEntityOffsetDirection,
			dx::XMVector3Normalize(
				this->followEntityOffset.loadXMVECTOR()
			)
		);

		this->followEntityOffset.x += followEntityOffsetDirection.x * (zoomDirection * this->zoomFactor);
		this->followEntityOffset.y += followEntityOffsetDirection.y * (zoomDirection * this->zoomFactor);
		this->followEntityOffset.z += followEntityOffsetDirection.z * (zoomDirection * this->zoomFactor);

		float distToCenter = abs(this->followEntityOffset.x) + abs(this->followEntityOffset.y) + abs(this->followEntityOffset.z);
		if(distToCenter < this->minDistanceToEntity || distToCenter > this->maxDistanceToEntity) {
			this->followEntityOffset.x -= followEntityOffsetDirection.x * (zoomDirection * this->zoomFactor);
			this->followEntityOffset.y -= followEntityOffsetDirection.y * (zoomDirection * this->zoomFactor);
			this->followEntityOffset.z -= followEntityOffsetDirection.z * (zoomDirection * this->zoomFactor);
		}
	}
}


void Camera::setPerspectiveProjection(float fieldOfView, float aspectRatio, float nearZ, float farZ) {
	this->projectionType = PERSPECTIVE;

	this->setNearPlaneDistance(nearZ);
	this->setFarPlaneDistance(farZ);

	// Perspective
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.projectionMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixPerspectiveFovLH(
			((float)fieldOfView / 360) * (dx::XM_2PI),
				aspectRatio,
				nearZ,
				farZ
			)
		)
	);
	this->shouldUpdateGPUData = true;
}

void Camera::setOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ) {
	this->projectionType = ORTHOGRAPHIC;

	this->setNearPlaneDistance(nearZ);
	this->setFarPlaneDistance(farZ);

	// Orthographic
	dx::XMStoreFloat4x4(
		&this->gCameraVSConstantBuffer.projectionMatrix,
		dx::XMMatrixTranspose(
			dx::XMMatrixOrthographicLH(
				viewWidth, viewHeight,
				nearZ, farZ
			)
		)
	);
	this->shouldUpdateGPUData = true;
}