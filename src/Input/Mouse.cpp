#include "Mouse.h"

Mouse::Mouse(){
	this->pRect = new RECT({ 0 });
}

// Game Engine
void Mouse::Reset(){
	// Keep previous state.
	memcpy(&this->buttonPreviousStates, &this->buttonStates, (sizeof(bool) * MOUSE_BUTTON_COUNT));

	this->rawAccumulateX = 0;
	this->rawAccumulateY = 0;
	this->wheelRotateCountDirection = 0;
}

bool Mouse::isButtonPressed(MOUSE_BUTTON button) {
	return this->buttonStates[button] && !this->buttonPreviousStates[button];
}

bool Mouse::isButtonHeld(MOUSE_BUTTON button) {
	return this->buttonStates[button] && this->buttonPreviousStates[button];
}

bool Mouse::isButtonReleased(MOUSE_BUTTON button) {
	return !this->buttonStates[button] && this->buttonPreviousStates[button];
}

bool Mouse::confineCursor(){
	bool success = ClipCursor(this->pRect);
	if (success) {
		this->hideCursor();
		this->confined = true;
	}else {
		this->confined = false;
	}
	return success;
}

bool Mouse::freeCursor(){
	bool success = ClipCursor(NULL);
	if (success) {
		this->showCursor();
		this->confined = false;
	}
	return success;
}

void Mouse::hideCursor(){
	int rCursor;
	do {
		rCursor = ShowCursor(false);
	} while (rCursor >= 0);
}

void Mouse::showCursor(){
	ShowCursor(true);
}

// Windows procedure
void Mouse::OnLeftPress(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	this->buttonStates[MOUSE_BUTTON::LEFT_BUTTON] = true;
}

void Mouse::OnLeftRelease(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	this->buttonStates[MOUSE_BUTTON::LEFT_BUTTON] = false;
}

void Mouse::OnRightPress(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	this->buttonStates[MOUSE_BUTTON::RIGHT_BUTTON] = true;
}

void Mouse::OnRightRelease(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	this->buttonStates[MOUSE_BUTTON::RIGHT_BUTTON] = false;
}

// Move callbacks.
void Mouse::OnMove(WPARAM wParam, LPARAM lParam) {
	if (!this->confined){
		this->posX = GET_X_LPARAM(lParam);
		this->posY = GET_Y_LPARAM(lParam);
	}
}

void Mouse::OnMoveRaw(signed long deltaX, signed long deltaY){
	if (this->confined) {
		this->rawAccumulateX += deltaX;
		this->rawAccumulateY += deltaY;
	}
}

void Mouse::OnWheelMove(WPARAM wParam, LPARAM lParam){
	this->wheelAccumulate += GET_WHEEL_DELTA_WPARAM(wParam);

	while(this->wheelAccumulate >= WHEEL_DELTA) {
		this->wheelAccumulate -= WHEEL_DELTA;
		this->wheelRotateCountDirection--;
	}
	while(this->wheelAccumulate <= -WHEEL_DELTA) {
		this->wheelAccumulate += WHEEL_DELTA;
		this->wheelRotateCountDirection++;
	}
}