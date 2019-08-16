#include "Mouse.h"

Mouse::Mouse() {
	this->x = 0;
	this->y = 0;
	this->wheelAccumulate = 0;
	this->leftPressed = false;
	this->rightPressed = false;
}

// Game Engine
int Mouse::getX()
{
	return this->x;
}

int Mouse::getY()
{
	return this->y;
}

// Windows procedure
void Mouse::OnLeftPress(WPARAM wParam, LPARAM lParam){
	this->x = GET_X_LPARAM(lParam);
	this->y = GET_Y_LPARAM(lParam);
	leftPressed = true;
}

void Mouse::OnLeftRelease(WPARAM wParam, LPARAM lParam){
	this->x = GET_X_LPARAM(lParam);
	this->y = GET_Y_LPARAM(lParam);
	leftPressed = false;
}

void Mouse::OnRightPress(WPARAM wParam, LPARAM lParam){
	this->x = GET_X_LPARAM(lParam);
	this->y = GET_Y_LPARAM(lParam);
	rightPressed = true;
}

void Mouse::OnRightRelease(WPARAM wParam, LPARAM lParam){
	this->x = GET_X_LPARAM(lParam);
	this->y = GET_Y_LPARAM(lParam);
	rightPressed = false;
}

void Mouse::OnMove(WPARAM wParam, LPARAM lParam){
	this->x = GET_X_LPARAM(lParam);
	this->y = GET_Y_LPARAM(lParam);
}

void Mouse::OnWheelMove(WPARAM wParam, LPARAM lParam){
	this->wheelAccumulate += GET_WHEEL_DELTA_WPARAM(wParam);
}

void Mouse::OnLeave(WPARAM wParam, LPARAM lParam){

}

void Mouse::OnHover(WPARAM wParam, LPARAM lParam){

}