#include "Controller.h"

void Controller::Setup(World* world, Keyboard* keyb, Mouse* mouse) {
	this->pWorld = world;
	this->pKeyb = keyb;
	this->pMouse = mouse;
}

void Controller::Update() {
	//// Keyboard
	// Free cursor with ESC key.
	if (this->pKeyb->isKeyPressed(VK_ESCAPE)) {
		this->pMouse->freeCursor();
	}

	// Switch camera with TAB
	if (this->pKeyb->isKeyPressed(VK_TAB)) {
		this->pWorld->switchCamera();
	}

	// Activate ragdoll physics on main character's entity.
	if (pMainCharacter != NULL) {
		if (this->pKeyb->isKeyPressed('F')) {
			this->pMainCharacter->meshDeformer->activateRagdoll();
		}
		if (this->pKeyb->isKeyPressed('G')) {
			this->pMainCharacter->useNormalMapping = !this->pMainCharacter->useNormalMapping;
			this->pMainCharacter->dataChanged = true;
		}
	}

	// Control character.
	if (this->pWorld->activeCamera->isFollowingEntity) {
		if (this->pKeyb->isKeyPressed('W') || this->pKeyb->isKeyPressed(VK_UP)) {
			this->pMainCharacter->Walk(
				Vector3(0.0f, 0.0f, -1.0f)
			);
		}
		if (this->pKeyb->isKeyPressed('S') || this->pKeyb->isKeyPressed(VK_DOWN)) {
			this->pMainCharacter->Walk(
				Vector3(0.0f, 0.0f, 1.0f)
			);
		}
		if (this->pKeyb->isKeyPressed('A') || this->pKeyb->isKeyPressed(VK_LEFT)) {
			this->pMainCharacter->Walk(
				Vector3(1.0f, 0.0f, 0.0f)
			);
		}
		if (this->pKeyb->isKeyPressed('D') || this->pKeyb->isKeyPressed(VK_RIGHT)) {
			this->pMainCharacter->Walk(
				Vector3(-1.0f, 0.0f, 0.0f)
			);
		}
	}
	// Camera position if camera isn't following any entity.
	else {
		if (this->pKeyb->isKeyPressed('W') || this->pKeyb->isKeyPressed(VK_UP)) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, 0.0f, 1.0f),
				this->pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyPressed('S') || this->pKeyb->isKeyPressed(VK_DOWN)) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, 0.0f, -1.0f),
				this->pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyPressed('A') || this->pKeyb->isKeyPressed(VK_LEFT)) {
			this->pWorld->activeCamera->Move(
				Vector3(-1.0f, 0.0f, 0.0f),
				this->pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyPressed('D') || this->pKeyb->isKeyPressed(VK_RIGHT)) {
			this->pWorld->activeCamera->Move(
				Vector3(1.0f, 0.0f, 0.0f),
				this->pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyPressed('E')) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, 1.0f, 0.0f),
				false
			);
		}
		if (this->pKeyb->isKeyPressed('Q')) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, -1.0f, 0.0f),
				false
			);
		}
	}

	//// Mouse
	// Camera rotation (Pitch, Yaw)
	if (this->pMouse->rawAccumulateX != 0 || this->pMouse->rawAccumulateY != 0) {
		this->pWorld->activeCamera->Rotate((float) this->pMouse->rawAccumulateX, (float) this->pMouse->rawAccumulateY);
	}

	// Camera zoom in/out
	if (this->pWorld->activeCamera->isFollowingEntity && this->pMouse->wheelRotateCountDirection != 0) {
		this->pWorld->activeCamera->Zoom(this->pMouse->wheelRotateCountDirection);
	}
}

void Controller::Reset() {
	this->pMouse->Reset();
}

void Controller::setMainCharacter(Character* character) {
	this->pMainCharacter = character;
}