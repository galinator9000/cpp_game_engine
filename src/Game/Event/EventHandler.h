#pragma once
#include "Structs.h"
#include "Event.h"

class EventHandler {
public:
	void Update();
	std::vector<Event*> pEventQueue;
};