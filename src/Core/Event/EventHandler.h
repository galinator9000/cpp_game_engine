#pragma once
#include "Structs.h"
#include "Event.h"

#include <queue>

class EventHandler {
public:
	void Update();
	void Reset();
	void pushEvent(Event* event);

	std::queue<Event*> pEventQueue;
};