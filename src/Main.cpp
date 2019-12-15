#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Main window class
	Window* pMainWnd = new Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	// InputHandler class which is a wrapper for keyboard and mouse classes.
	InputHandler* pInputHandler = new InputHandler();

	// Input to window
	Keyboard* pKeyb = new Keyboard();
	Mouse* pMouse = new Mouse();

	// Create graphics and physics component
	Graphics* pGfx = new Graphics(
		pMainWnd->GetHandler(),
		WIDTH,
		HEIGHT,
		GRAPHICS_REFRESH_RATE
	);
	Physics* pPhy = new Physics(GRAVITY, PHYSICS_REFRESH_RATE);
	Character::worldGravity = GRAVITY / PHYSICS_REFRESH_RATE;

	// Initialize audio component
	Audio* pAud = new Audio();

	// Create world and game classes.
	// World is integrates physics and graphics components.
	World* pWorld = new World(pGfx, pPhy, pAud);
	pWorld->Setup();

	// Game is setups scene.
	Game* pGame = new Game(pWorld, pInputHandler);
	pGame->Setup();

	// Connect window and controller with keyboard and mouse.
	pMainWnd->Setup(pKeyb, pMouse);
	pInputHandler->Setup(pWorld, pKeyb, pMouse);

	// Main loop of the engine
	while (pMainWnd->ProcessMessages()){
		// Update section
		pInputHandler->Update();
		pGame->Update();
		pWorld->Update();

		// Reset section
		pWorld->Reset();
		pGame->Reset();
		pInputHandler->Reset();
	}

	return 0;
}