#include "Mouse.h"

Mouse::Mouse(){}

// Game Engine
void Mouse::resetRawAccumulate(){
	this->rawAccumulateX = 0;
	this->rawAccumulateY = 0;
}

// Windows procedure
void Mouse::OnLeftPress(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	leftPressed = true;
}

void Mouse::OnLeftRelease(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	leftPressed = false;
}

void Mouse::OnRightPress(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	rightPressed = true;
}

void Mouse::OnRightRelease(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
	rightPressed = false;
}

void Mouse::OnMove(WPARAM wParam, LPARAM lParam){
	this->posX = GET_X_LPARAM(lParam);
	this->posY = GET_Y_LPARAM(lParam);
}

void Mouse::OnMoveRaw(signed long deltaX, signed long deltaY){
	this->rawAccumulateX += deltaX;
	this->rawAccumulateY += deltaY;
}

void Mouse::OnWheelMove(WPARAM wParam, LPARAM lParam){
	this->wheelAccumulate += GET_WHEEL_DELTA_WPARAM(wParam);
}

void Mouse::OnLeave(WPARAM wParam, LPARAM lParam){

}

void Mouse::OnHover(WPARAM wParam, LPARAM lParam){

}