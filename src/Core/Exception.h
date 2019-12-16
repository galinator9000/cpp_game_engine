#pragma once
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#include <Windowsx.h>

class Exception{
	Exception(LPCTSTR errorText);
};