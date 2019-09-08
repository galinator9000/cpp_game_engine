#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Main window class
	Window* pMainWnd = new Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	// Controller class which is a wrapper for keyboard and mouse classes.
	Controller* pController = new Controller();

	// Input to window
	Keyboard* pKeyb = new Keyboard();
	Mouse* pMouse = new Mouse();

	// Create graphics and physics component
	Graphics* pGfx = new Graphics(
		pMainWnd->GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);
	Physics* pPhy = new Physics({ 0.0f, -9.81f, 0.0f }, 60.0f);

	// Entities creates their own physical representations.
	Entity::ppxPhysics = pPhy->pxPhysics;

	// Create world and game classes.
	// World is integrates physics and graphics components.
	World* pWorld = new World(pGfx, pPhy);
	pWorld->Setup();

	// Game is setups scene.
	Game* pGame = new Game(pWorld);
	pGame->Setup();

	// Connect window and controller with keyboard and mouse.
	pMainWnd->Setup(pKeyb, pMouse);
	pController->Setup(pWorld, pKeyb, pMouse);

	// Main loop of the engine
	while (pMainWnd->ProcessMessages()){
		// Update section
		pWorld->Update();
		pGame->Update();
		pController->Update();

		// Reset section
		pController->Reset();
		pWorld->Reset();
	}

	return 0;
}