#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Setup engine
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

	pMainWnd->keyb = pKeyb;
	pMainWnd->mouse = pMouse;

	// Create graphics and physics components
	Graphics* pGfx = new Graphics(
		pMainWnd->GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);
	Physics* pPhy = new Physics(-9.81f, 60.0f);
	BaseEntity::ppxPhysics = pPhy->pxPhysics;

	// Create world.
	World* pWorld = new World(pGfx, pPhy);

	// Use engine
	Game* pGame = new Game(pWorld);
	pGame->Setup();

	// Main loop of the engine
	while (pMainWnd->ProcessMessages()){
		// Update section
		pWorld->Update();
		pGame->Update();

		// Input handling section
		// Keyboard
		// Camera position
		if (pKeyb->isKeyPressed('W') || pKeyb->isKeyPressed(VK_UP)) {
			pWorld->activeCamera->Move(
				0.0f, 0.0f, 1.0f,
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed('S') || pKeyb->isKeyPressed(VK_DOWN)) {
			pWorld->activeCamera->Move(
				0.0f, 0.0f, -1.0f,
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed('A') || pKeyb->isKeyPressed(VK_LEFT)) {
			pWorld->activeCamera->Move(
				-1.0f, 0.0f, 0.0f,
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed('D') || pKeyb->isKeyPressed(VK_RIGHT)) {
			pWorld->activeCamera->Move(
				1.0f, 0.0f, 0.0f,
				pKeyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (pKeyb->isKeyPressed(' ')) {
			pWorld->activeCamera->Move(
				0.0f, 1.0f, 0.0f,
				false
			);
		}
		if (pKeyb->isKeyPressed('C')) {
			pWorld->activeCamera->Move(
				0.0f, -1.0f, 0.0f,
				false
			);
		}

		// Mouse
		// Camera rotation (Pitch, Yaw)
		if(pMouse->rawAccumulateX != 0 || pMouse->rawAccumulateY != 0){
			pWorld->activeCamera->Rotate((float) pMouse->rawAccumulateX, (float)pMouse->rawAccumulateY);
		}

		/*std::ostringstream myStream;
		myStream << world->activeCamera->wasMovingFast << ", ";
		myStream << world->activeCamera->currentMovementSpeed << "\n";
		OutputDebugStringA(myStream.str().c_str());*/

		// Resetting section.
		pMouse->resetRawAccumulate();
	}

	return 0;
}