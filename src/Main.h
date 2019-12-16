#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>
#include <wrl.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

// Main components
#include "Core/Window.h"
#include "Graphics/Graphics.h"
#include "Physics/Physics.h"
#include "Audio/Audio.h"

// Game world
#include "Game.h"
#include "World.h"
#include "Input/InputHandler.h"
#include "Character/Character.h"

#include "Structs.h"
#include "Config.h"