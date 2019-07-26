#include "Mouse.h"

Mouse::Mouse() {
	this->x = 0;
	this->y = 0;
	this->wheelAccumulate = 0;
	this->leftPressed = false;
	this->rightPressed = false;
}

void Mouse::OnLeftPress(int xx, int yy){
	this->x = xx;
	this->y = yy;
	leftPressed = true;
}

void Mouse::OnLeftRelease(int xx, int yy){
	this->x = xx;
	this->y = yy;
	leftPressed = false;
}

void Mouse::OnRightPress(int xx, int yy){
	this->x = xx;
	this->y = yy;
	rightPressed = true;
}

void Mouse::OnRightRelease(int xx, int yy){
	this->x = xx;
	this->y = yy;
	rightPressed = false;
}

void Mouse::OnMove(int xx, int yy){
	this->x = xx;
	this->y = yy;
}

void Mouse::OnWheelMove(int xx, int yy, int wheelDelta){
	this->wheelAccumulate += wheelDelta;
}

void Mouse::OnLeave(){

}

void Mouse::OnHover(){

}

int Mouse::GetX()
{
	return this->x;
}

int Mouse::GetY()
{
	return this->y;
}
