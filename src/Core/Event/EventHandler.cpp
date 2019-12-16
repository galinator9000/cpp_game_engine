#include "EventHandler.h"

void EventHandler::Update() {
	for (unsigned int e = 0; e < this->pEventQueue.size(); e++) {
		Event* curEvent = this->pEventQueue.front();

		// Process current event..

		this->pEventQueue.pop();
	}
}

void EventHandler::Reset() {

}

void EventHandler::pushEvent(Event* event) {
	this->pEventQueue.push(event);
}