#pragma once
#include <Windows.h>
#include <Windowsx.h>
#include <wrl.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "Window.h"
#include "Graphics.h"

#define _DEBUGGING TRUE

enum EXIT {
	USER_EXIT
};

const auto WND_TITLE = L"Direct3D";
const int WIDTH = 800;
const int HEIGHT = 600;
const int REFRESH_RATE = 60;