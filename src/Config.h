#pragma once
#include "Structs.h"

#define _DEBUGGING FALSE
namespace{
	const auto WND_TITLE = L"Direct3D";
	const int WIDTH = 1024;
	const int HEIGHT = 768;
	const int GRAPHICS_REFRESH_RATE = 60;
	const float PHYSICS_REFRESH_RATE = 60.0f;
	const int FOV = 90;

	// Gray background color.
	const Color BACKGROUND_COLOR = Color(0.14f, 0.14f, 0.14f, 1.0f);
	Vector3 GRAVITY = Vector3(0.0f, -9.81f, 0.0f);
}