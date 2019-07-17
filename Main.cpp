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

	while (true) {
		// Terminal condition of the program.
		if (!mainWnd.ProcessMessages()) {
			break;
		}

		gfx.UpdateFrame();
	}

	return 0;
}