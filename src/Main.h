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
#include "Window.h"
#include "Graphics.h"
#include "Physics.h"
#include "Audio.h"

// Game world
#include "Game.h"
#include "World.h"
#include "Controller.h"
#include "Entity/Character/Character.h"

#include "Structs.h"
#include "Config.h"