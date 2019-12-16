#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

#include "World.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Character/Character.h"

class InputHandler{
public:
	void Setup(World* world, Keyboard* keyb, Mouse* mouse);
	void Update();
	void Reset();

	World* pWorld;
	Keyboard* pKeyb;
	Mouse* pMouse;

	// Main character pointer
	Character* pMainCharacter = NULL;
	void setMainCharacter(Character* character);
};