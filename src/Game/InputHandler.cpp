#include "InputHandler.h"

void InputHandler::Setup(World* world, Keyboard* keyb, Mouse* mouse) {
	this->pWorld = world;
	this->pKeyb = keyb;
	this->pMouse = mouse;
}

void InputHandler::Update() {
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
	}

	// Character Camera / Free Camera movement
	if (this->pMainCharacter && this->pMainCharacter->characterCamera && this->pWorld->activeCamera->id == this->pMainCharacter->characterCamera->id) {
		if (this->pKeyb->isKeyPressed('W') || this->pKeyb->isKeyPressed(VK_UP)) {
			this->pMainCharacter->Walk(
				Vector3(0.0f, 0.0f, 1.0f)
			);
		}
		if (this->pKeyb->isKeyPressed('S') || this->pKeyb->isKeyPressed(VK_DOWN)) {
			this->pMainCharacter->Walk(
				Vector3(0.0f, 0.0f, -1.0f)
			);
		}
		if (this->pKeyb->isKeyPressed('A') || this->pKeyb->isKeyPressed(VK_LEFT)) {
			this->pMainCharacter->Walk(
				Vector3(-1.0f, 0.0f, 0.0f)
			);
		}
		if (this->pKeyb->isKeyPressed('D') || this->pKeyb->isKeyPressed(VK_RIGHT)) {
			this->pMainCharacter->Walk(
				Vector3(1.0f, 0.0f, 0.0f)
			);
		}
	}
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
}

void InputHandler::Reset() {
	this->pKeyb->Reset();
	this->pMouse->Reset();
}

void InputHandler::setMainCharacter(Character* character) {
	this->pMainCharacter = character;
}