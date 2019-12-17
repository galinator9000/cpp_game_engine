#include "Keyboard.h"

Keyboard::Keyboard(){

}

// Game Engine
void Keyboard::Reset() {
	// Keep previous state.
	memcpy(&this->keyPreviousState, &this->keyState, this->keyState.size());

	//memset(&this->keyState, false, this->keyState.size());
}

bool Keyboard::isKeyPressed(unsigned int key) {
	return this->keyState[key] && !this->keyPreviousState[key];
}

bool Keyboard::isKeyHeld(unsigned int key) {
	return this->keyState[key] && this->keyPreviousState[key];
}

bool Keyboard::isKeyReleased(unsigned int key) {
	return !this->keyState[key] && this->keyPreviousState[key];
}

// Windows procedure
void Keyboard::OnKeyPress(WPARAM wParam, LPARAM lParam) {
	this->keyState[wParam] = true;
}

void Keyboard::OnKeyRelease(WPARAM wParam, LPARAM lParam) {
	this->keyState[wParam] = false;
}

void Keyboard::OnChar(WPARAM wParam, LPARAM lParam) {

}