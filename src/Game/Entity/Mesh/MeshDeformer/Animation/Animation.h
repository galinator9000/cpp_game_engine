#pragma once
#include "Keyframe.h"
#include <string>
#include <vector>

class Animation {
public:
	std::string name = "";

	std::vector<Keyframe*> gKeyFrames;
	unsigned int gKeyframeCount;

	// Frame rate per-second.
	float frameRate;
	float duration;
};