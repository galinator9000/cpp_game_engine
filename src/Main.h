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

// Game world
#include "Game.h"
#include "World.h"

#define _DEBUGGING TRUE

const auto WND_TITLE = L"Direct3D";
const int WIDTH = 1024;
const int HEIGHT = 768;
const int REFRESH_RATE = 60;
const int FOV = 90;