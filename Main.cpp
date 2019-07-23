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

	float angle = 0.0f;
	float fmx, fmy;
	while (true){
		// Terminal condition of the program.
		if (!mainWnd.ProcessMessages()) {
			break;
		}

		fmx = mainWnd.mouse.GetX() / (WIDTH / 2.0f) - 1.0f;
		fmy = -mainWnd.mouse.GetY() / (HEIGHT / 2.0f) + 1.0f;

		gfx.BeginFrame();
		gfx.EndFrame(fmx, fmy, angle);
		angle += 0.01f;
	}

	return 0;
}