#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Main window class
	Window* pMainWnd = new Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	// Input to window
	Keyboard* pKeyb = new Keyboard();
	Mouse* pMouse = new Mouse();
	pMainWnd->Setup(pKeyb, pMouse);

	// Create graphics and physics component
	Graphics* pGfx = new Graphics(
		pMainWnd->GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);
	Physics* pPhy = new Physics({ 0.0f, -9.81f, 0.0f }, 60.0f);

	// Entities creates their own physics representation.
	Entity::ppxPhysics = pPhy->pxPhysics;

	// Create world.
	World* pWorld = new World(pGfx, pPhy);
	pWorld->Setup();

	Game* pGame = new Game(pWorld);
	pGame->Setup();

	// Main loop of the engine
	while (pMainWnd->ProcessMessages()){
		//// Update section
		pWorld->Update();
		pGame->Update();

		//// Input handling section
		// Keyboard
		if (pKeyb->isKeyPressed(VK_ESCAPE)) {
			pMouse->freeCursor();
		}

		// Switch camera with TAB
		if (pKeyb->isKeyPressed(VK_TAB)) {
			pWorld->switchCamera();
		}

		// Camera position
		if (pKeyb->isKeyPressed('W') || pKeyb->isKeyPressed(VK_UP)) {
			pWorld->activeCamera->Move(
				Vector3(0.0f, 0.0f, 1.0f),
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed('S') || pKeyb->isKeyPressed(VK_DOWN)) {
			pWorld->activeCamera->Move(
				Vector3(0.0f, 0.0f, -1.0f),
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed('A') || pKeyb->isKeyPressed(VK_LEFT)) {
			pWorld->activeCamera->Move(
				Vector3(-1.0f, 0.0f, 0.0f),
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed('D') || pKeyb->isKeyPressed(VK_RIGHT)) {
			pWorld->activeCamera->Move(
				Vector3(1.0f, 0.0f, 0.0f),
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed(' ')) {
			pWorld->activeCamera->Move(
				Vector3(0.0f, 1.0f, 0.0f),
				false
			);
		}
		if (pKeyb->isKeyPressed('C')) {
			pWorld->activeCamera->Move(
				Vector3(0.0f, -1.0f, 0.0f),
				false
			);
		}

		// Mouse
		// Camera rotation (Pitch, Yaw)
		if(pMouse->rawAccumulateX != 0 || pMouse->rawAccumulateY != 0){
			pWorld->activeCamera->Rotate((float) pMouse->rawAccumulateX, (float)pMouse->rawAccumulateY);
		}

		//// Reset section.
		pMouse->resetRawAccumulate();
		pWorld->Reset();
	}

	return 0;
}