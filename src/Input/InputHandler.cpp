#include "InputHandler.h"

void InputHandler::Setup(Keyboard* keyb, Mouse* mouse, EventHandler* eventHandler, World* world) {
	this->pKeyb = keyb;
	this->pMouse = mouse;
	this->pEventHandler = eventHandler;
	this->pWorld = world;
}

void InputHandler::Update() {
	//// Keyboard
	// Free cursor with ESC key.
	if (this->pKeyb->isKeyReleased(VK_ESCAPE)) {
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
		if (this->pKeyb->isKeyHeld('W') || this->pKeyb->isKeyHeld(VK_UP)) {
			this->pMainCharacter->Walk(
				Vector3(0.0f, 0.0f, 1.0f)
			);
		}
		if (this->pKeyb->isKeyHeld('S') || this->pKeyb->isKeyHeld(VK_DOWN)) {
			this->pMainCharacter->Walk(
				Vector3(0.0f, 0.0f, -1.0f)
			);
		}
		if (this->pKeyb->isKeyHeld('A') || this->pKeyb->isKeyHeld(VK_LEFT)) {
			this->pMainCharacter->Walk(
				Vector3(-1.0f, 0.0f, 0.0f)
			);
		}
		if (this->pKeyb->isKeyHeld('D') || this->pKeyb->isKeyHeld(VK_RIGHT)) {
			this->pMainCharacter->Walk(
				Vector3(1.0f, 0.0f, 0.0f)
			);
		}
	}
	else {
		if (this->pKeyb->isKeyHeld('W') || this->pKeyb->isKeyHeld(VK_UP)) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, 0.0f, 1.0f),
				this->pKeyb->isKeyHeld(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyHeld('S') || this->pKeyb->isKeyHeld(VK_DOWN)) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, 0.0f, -1.0f),
				this->pKeyb->isKeyHeld(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyHeld('A') || this->pKeyb->isKeyHeld(VK_LEFT)) {
			this->pWorld->activeCamera->Move(
				Vector3(-1.0f, 0.0f, 0.0f),
				this->pKeyb->isKeyHeld(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyHeld('D') || this->pKeyb->isKeyHeld(VK_RIGHT)) {
			this->pWorld->activeCamera->Move(
				Vector3(1.0f, 0.0f, 0.0f),
				this->pKeyb->isKeyHeld(VK_CONTROL)
			);
		}
		if (this->pKeyb->isKeyHeld('E')) {
			this->pWorld->activeCamera->Move(
				Vector3(0.0f, 1.0f, 0.0f),
				false
			);
		}
		if (this->pKeyb->isKeyHeld('Q')) {
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