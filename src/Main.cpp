#include "Main.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	Window mainWnd = Window(
		hInstance,
		WND_TITLE,
		WIDTH,
		HEIGHT
	);

	Graphics gfx = Graphics(
		mainWnd.GetHandler(),
		WIDTH,
		HEIGHT,
		REFRESH_RATE
	);

	Physics phy = Physics(-10, 60.0f);

	float angle = 0.0f;
	float fmx, fmy;

	// Main loop of the engine.
	while (true){
		// Terminal condition of the engine.
		if (!mainWnd.ProcessMessages()) {
			break;
		}
		
		// Get mouse coordinates.
		fmx = mainWnd.mouse.GetX() / (WIDTH / 2.0f) - 1.0f;
		fmy = -mainWnd.mouse.GetY() / (HEIGHT / 2.0f) + 1.0f;

		// Clear frame and redraw state of the world.
		gfx.BeginFrame();
		gfx.Draw(0, 0, angle);
		gfx.Draw(0, 0, -angle);
		gfx.EndFrame();
		angle += 0.01f;

		// Step and update physics of the world.
		phy.Update();
	}

	return 0;
}