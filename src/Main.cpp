#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Main window class
	Window* mainWnd = new Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	// Input to window
	Keyboard* keyb = new Keyboard();
	Mouse* mouse = new Mouse();

	mainWnd->keyb = keyb;
	mainWnd->mouse = mouse;

	// Create graphics and physics components
	Graphics* gfx = new Graphics(
		mainWnd->GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);
	Physics* phy = new Physics(-9.81f, 60.0f);
	BaseEntity::ppxPhysics = phy->pxPhysics;

	// Create world.
	World* world = new World(gfx, phy);

	// Add main camera.
	Camera* camera = new Camera(
		0.0f, 3.0f, 1.0f,
		FOV,
		WIDTH / HEIGHT
	);
	world->addCamera(camera, true);

	// Add static ground.
	Plane* ground = new Plane(
		PxVec3(10.0f, 10.0f, 0)
	);
	world->addEntity(ground);

	// Add dynamic cube.
	Box* box = new Box(
		PxVec3(1.5f, 0.05f, 1.5f),
		PxVec3(0.0f, 2.0f, 6.0f),
		PxVec3(10.0f, 0.0f, 10.0f),
		PxVec3(0, 0, 0)
	);
	world->addEntity(box);

	// Add small cubes.
	for (int i = 1; i<500; i++) {
		Box* box2 = new Box(
			PxVec3(0.01f, 1.0f, 0.01f),
			PxVec3(i*0.01f, i*5.0f, 6.0f),
			PxVec3(0.0f, 0.0f, 0.0f),
			PxVec3(0, 0, 0)
		);
		world->addEntity(box2);
	}

	// Main loop of the engine
	while (mainWnd->ProcessMessages()){
		////// Update section
		world->Update();

		////// Input handling section
		// Keyboard
		if (keyb->isKeyPressed('W')) {
			world->activeCamera->moveDirection(0.0f, 0.0f, 1.0f);
		}
		if (keyb->isKeyPressed('S')) {
			world->activeCamera->moveDirection(0.0f, 0.0f, -1.0f);
		}
		if (keyb->isKeyPressed('A')) {
			world->activeCamera->moveDirection(-1.0f, 0.0f, 0.0f);
		}
		if (keyb->isKeyPressed('D')) {
			world->activeCamera->moveDirection(1.0f, 0.0f, 0.0f);
		}
		if (keyb->isKeyPressed(' ')) {
			world->activeCamera->moveDirection(0.0f, 1.0f, 0.0f);
		}
		if (keyb->isKeyPressed('C')) {
			world->activeCamera->moveDirection(0.0f, -1.0f, 0.0f);
		}

		// Mouse

		/*std::ostringstream myStream;
		myStream << mouse->rawAccumulateX << ", ";
		myStream << mouse->rawAccumulateY << "\n";
		OutputDebugStringA(myStream.str().c_str());*/

		////// Resetting section.
		mouse->resetRawAccumulate();
	}

	return 0;
}