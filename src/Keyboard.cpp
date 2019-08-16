#include "Keyboard.h"

Keyboard::Keyboard(){}

// Game Engine
bool Keyboard::isKeyPressed(unsigned int key) {
	return this->keyStatus[key];
}

// Windows procedure
void Keyboard::OnKeyPress(WPARAM wParam, LPARAM lParam) {
	this->keyStatus[wParam] = true;
}

void Keyboard::OnKeyRelease(WPARAM wParam, LPARAM lParam) {
	this->keyStatus[wParam] = false;

}

void Keyboard::OnChar(WPARAM wParam, LPARAM lParam) {

}
