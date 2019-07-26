#pragma once
#include "Windows.h"

class Keyboard{
public:
	Keyboard();
	void OnKeyPress();
	void OnKeyRelease();
	void OnChar();
};