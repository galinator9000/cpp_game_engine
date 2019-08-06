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

	// Add ground.
	/*
	Cube ground = Cube(btVector3(0.0f, 0.0f, 0.0f), btQuaternion(0.0f, 0.0f, 0.0f), btVector3(50.0f, 1.0f, 50.0f), 0.0f);
	world.addEntity(ground);

	// Add dynamic cube.
	Cube cube = Cube(btVector3(0.0f, 25.0f, 0.0f), btQuaternion(45.0f, 45.0f, 45.0f), btVector3(1.0f, 1.0f, 1.0f), 3.0f);
	world.addEntity(cube);
	*/

	// Main loop of the engine.
	while (true){
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

		// Update world.
		world.Update();
	}

	return 0;
}