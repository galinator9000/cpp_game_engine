#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	Window mainWnd = Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	// Create render and physics components, give it to world component.
	Graphics gfx = Graphics(
		mainWnd.GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);
	Physics phy = Physics(-10.0f, 60.0f);
	World world = World(&gfx, &phy);

	BaseEntity::ppxPhysics = phy.pxPhysics;

	// Add static ground.
	Plane ground = Plane(
		PxVec3(0.5f, 0.5f, 0.6f)
	);
	world.addEntity(ground);

	// Add dynamic cube.
	Box box = Box(
		PxVec3(15.0f, 15.0f, 15.0f),
		PxVec3(0.0f, 10.0f, 0.0f),
		PxVec3(45.0f, 45.0f, 45.0f),
		PxVec3(0, 0, 0)
	);
	world.addEntity(box);

	// Main loop of the engine.
	while (true){
		// Update world.
		world.Update();

		// Terminal condition of the engine.
		if (!mainWnd.ProcessMessages()) {
			break;
		}

		/*
		Print mouse XY.
		std::ostringstream myStream;
		myStream << "X: " << mainWnd.mouse.GetX();
		myStream << "Y: " << mainWnd.mouse.GetY() << "\n";
		OutputDebugStringA(myStream.str().c_str());
		*/

		/*
		Print box's position.
		*/
		std::ostringstream myStream;
		myStream << float(box.rigidDynamic->getLinearVelocity().x) << ", ";
		myStream << float(box.rigidDynamic->getLinearVelocity().y) << ", ";
		myStream << float(box.rigidDynamic->getLinearVelocity().z) << "\n";
		OutputDebugStringA(myStream.str().c_str());
	}

	return 0;
}