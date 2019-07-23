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
	while (true){
		// Terminal condition of the program.
		if (!mainWnd.ProcessMessages()) {
			break;
		}

		gfx.BeginFrame();
		gfx.EndFrame(angle);
		angle += 0.01f;
	}

	return 0;
}