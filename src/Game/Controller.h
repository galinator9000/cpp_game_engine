#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

#include "Keyboard.h"
#include "Mouse.h"
#include "World.h"

class Controller{
public:
	void Setup(World* world, Keyboard* keyb, Mouse* mouse);
	void Update();
	void Reset();

	World* pWorld;
	Keyboard* pKeyb;
	Mouse* pMouse;
};