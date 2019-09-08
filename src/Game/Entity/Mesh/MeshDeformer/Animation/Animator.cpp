#include "Animator.h"

void Animator::Update(JointTransform** jointTransforms, unsigned int gJointCount) {
	// Keep animation progressing with usage of Timer.
	if (this->currentAnimation == NULL) return;

	// Reset timer when it reaches end so animation keeps playing over again.
	if (this->timer.Peek() > this->currentAnimation->duration) {
		this->timer.Reset();
	}

	float animTime = this->timer.Peek();
	float animationProgression = animTime / currentAnimation->duration;
	unsigned int currentKeyframeIndex = (unsigned int)(animationProgression * this->currentAnimation->gKeyframeCount);
	if (currentKeyframeIndex >= this->currentAnimation->gKeyframeCount) {
		currentKeyframeIndex = 0;
	}

	// Calculate next keyframe index.
	unsigned int nextKeyframeIndex = (currentKeyframeIndex + 1) % this->currentAnimation->gKeyframeCount;

	// Get keyframes.
	Keyframe* currentKeyFrame = this->currentAnimation->gKeyFrames[currentKeyframeIndex];
	Keyframe* nextKeyFrame = this->currentAnimation->gKeyFrames[nextKeyframeIndex];

	// Calculate frame progression. (Weighting while blending transformation matrices.)
	float totalTime = nextKeyFrame->timeStamp - currentKeyFrame->timeStamp;
	float currentTime = animTime - currentKeyFrame->timeStamp;
	float frameProgression = currentTime / totalTime;

	// Update joint animation matrices.
	for (unsigned int j = 0; j < gJointCount; j++) {
		// Directly set keyframe transformation.
		jointTransforms[j]->jointAnimTransformMatrix = *(currentKeyFrame->jointIDTransform[j]);

		// Or interpolate transformations between current and next keyframe.
		/*jointTransforms[j]->jointAnimTransformMatrix = this->interpolateTransformationMatrix(
			*(currentKeyFrame->jointIDTransform[j]),
			*(nextKeyFrame->jointIDTransform[j]),
			frameProgression
		);*/
	}
}

void Animator::setAnimation(Animation* animation) {
	this->currentAnimation = animation;
	this->timer.Reset();
}

dx::XMFLOAT4X4 Animator::interpolateTransformationMatrix(
	dx::XMFLOAT4X4 transformationCurrent,
	dx::XMFLOAT4X4 transformationNext,
	float progression
) {
	// Decompose Current matrix
	dx::XMVECTOR scalingCurrent;
	dx::XMVECTOR rotationQuatCurrent;
	dx::XMVECTOR translationCurrent;
	dx::XMMatrixDecompose(
		&scalingCurrent,
		&rotationQuatCurrent,
		&translationCurrent,
		dx::XMLoadFloat4x4(&transformationCurrent)
	);

	// Decompose Next matrix
	dx::XMVECTOR scalingNext;
	dx::XMVECTOR rotationQuatNext;
	dx::XMVECTOR translationNext;
	dx::XMMatrixDecompose(
		&scalingNext,
		&rotationQuatNext,
		&translationNext,
		dx::XMLoadFloat4x4(&transformationNext)
	);

	// Interpolate translation
	dx::XMVECTOR translationInterpolated = dx::XMVectorLerp(
		translationCurrent,
		translationNext,
		progression
	);

	// Interpolate quaternion
	dx::XMVECTOR rotationQuatInterpolated = dx::XMQuaternionSlerp(
		rotationQuatCurrent,
		rotationQuatNext,
		progression
	);

	// Store interpolated vectors.
	dx::XMFLOAT4X4* transformationInterpolated = new dx::XMFLOAT4X4;
	dx::XMStoreFloat4x4(
		transformationInterpolated,
		dx::XMMatrixRotationQuaternion(rotationQuatInterpolated) * dx::XMMatrixTranslationFromVector(translationInterpolated)
	);
	return *transformationInterpolated;
}