#include "Keyboard.h"

Keyboard::Keyboard(){}

void Keyboard::Reset() {
	//memset(&this->keyStatus, false, this->keyStatus.size());
}

// Game Engine
bool Keyboard::isKeyPressed(unsigned int key) {
	return this->keyStatus[key];
}

// Windows procedure
void Keyboard::OnKeyPress(WPARAM wParam, LPARAM lParam) {
	this->keyStatus[wParam] = true;
	unsigned int keyCount = (lParam & 0xFFFF);
	if (keyCount > 1) {
		keyCount++;
	}
}

void Keyboard::OnKeyRelease(WPARAM wParam, LPARAM lParam) {
	this->keyStatus[wParam] = false;
}

void Keyboard::OnChar(WPARAM wParam, LPARAM lParam) {

}
