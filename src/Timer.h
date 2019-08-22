#pragma once
#include <chrono>

class Timer{
public:
	Timer();
	void Reset();
	float Peek();

	std::chrono::steady_clock::time_point lastTimePoint;
};