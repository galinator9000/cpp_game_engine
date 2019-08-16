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
		//// Keyboard
		// Camera position
		if (keyb->isKeyPressed('W') || keyb->isKeyPressed(VK_UP)) {
			world->activeCamera->Move(
				0.0f, 0.0f, 1.0f,
				keyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (keyb->isKeyPressed('S') || keyb->isKeyPressed(VK_DOWN)) {
			world->activeCamera->Move(
				0.0f, 0.0f, -1.0f,
				keyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (keyb->isKeyPressed('A') || keyb->isKeyPressed(VK_LEFT)) {
			world->activeCamera->Move(
				-1.0f, 0.0f, 0.0f,
				keyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (keyb->isKeyPressed('D') || keyb->isKeyPressed(VK_RIGHT)) {
			world->activeCamera->Move(
				1.0f, 0.0f, 0.0f,
				keyb->isKeyPressed(VK_CONTROL)
			);
		}
		if (keyb->isKeyPressed(' ')) {
			world->activeCamera->Move(
				0.0f, 1.0f, 0.0f,
				false
			);
		}
		if (keyb->isKeyPressed('C')) {
			world->activeCamera->Move(
				0.0f, -1.0f, 0.0f,
				false
			);
		}

		//// Mouse
		// Camera rotation (Pitch, Yaw)
		if(mouse->rawAccumulateX != 0 || mouse->rawAccumulateY != 0){
			world->activeCamera->Rotate((float) mouse->rawAccumulateX, (float) mouse->rawAccumulateY);
		}

		std::ostringstream myStream;
		myStream << world->activeCamera->wasMovingFast << ", ";
		myStream << world->activeCamera->currentMovementSpeed << "\n";
		OutputDebugStringA(myStream.str().c_str());

		////// Resetting section.
		mouse->resetRawAccumulate();
	}

	return 0;
}