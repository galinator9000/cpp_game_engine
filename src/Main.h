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
#include "Game/World.h"

#define _DEBUGGING TRUE

enum EXIT {
	USER_EXIT
};

const auto WND_TITLE = L"Direct3D";
const int WIDTH = 1024;
const int HEIGHT = 768;
const int REFRESH_RATE = 60;