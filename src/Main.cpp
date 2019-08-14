#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	Window mainWnd = Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	// Create render and physics components, give it to world component.
	Graphics* gfx = new Graphics(
		mainWnd.GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);
	Physics* phy = new Physics(-9.81f, 60.0f);
	World* world = new World(gfx, phy);

	BaseEntity::ppxPhysics = phy->pxPhysics;

	// Add static ground.
	Plane* ground = new Plane(
		PxVec3(10.0f, 10.0f, 0)
	);
	world->addEntity(ground);

	// Add dynamic cube.
	Box* box = new Box(
		PxVec3(1.5f, 0.1f, 1.5f),
		PxVec3(0.0f, 2.0f, 6.0f),
		PxVec3(10.0f, 0.0f, 10.0f),
		PxVec3(0, 0, 0)
	);
	//world->addEntity(box);

	// Add small cubes.
	for (int i = 1; i<150; i++) {
		Box* box2 = new Box(
			PxVec3(0.01f, 0.1f, 0.01f),
			PxVec3(0.0f, i*5.0f, 6.0f),
			PxVec3(0.0f, 0.0f, 0.0f),
			PxVec3(0, 0, 0)
		);
		world->addEntity(box2);
	}

	// Main loop of the engine.
	while (true){
		// Update world.
		world->Update();

		// Terminal condition of the engine.
		if (!mainWnd.ProcessMessages()) {
			break;
		}

		/* Print box's position. */
		PxTransform tm = box->rigidDynamic->getGlobalPose();

		std::ostringstream myStream;
		myStream << "Pos: ";
		myStream << float(tm.p.x) << ", ";
		myStream << float(tm.p.y) << ", ";
		myStream << float(tm.p.z) << "\t";
		myStream << "Rot: ";
		myStream << float(tm.q.x) << ", ";
		myStream << float(tm.q.y) << ", ";
		myStream << float(tm.q.z) << "\n";
		OutputDebugStringA(myStream.str().c_str());
	}

	return 0;
}