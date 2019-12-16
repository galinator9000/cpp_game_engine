#include "Timer.h"

using namespace std::chrono;

Timer::Timer() {
	this->Reset();
}

// Set last timepoint.
void Timer::Reset(){
	this->lastTimePoint = steady_clock::now();
}

// Check how many seconds passed since last timepoint.
float Timer::Peek(){
	return duration<float>(steady_clock::now() - this->lastTimePoint).count();
}