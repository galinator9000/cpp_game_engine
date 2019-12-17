#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

#include "Core/Event/EventHandler.h"
#include "World.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Character/Character.h"

class InputHandler{
public:
	void Setup(Keyboard* keyb, Mouse* mouse, EventHandler* eventHandler, World* world);
	void Update();
	void Reset();

	Keyboard* pKeyb;
	Mouse* pMouse;
	EventHandler* pEventHandler;
	World* pWorld;

	// Main character pointer
	Character* pMainCharacter = NULL;
	void setMainCharacter(Character* character);
};