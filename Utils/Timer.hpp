#ifndef MICROLUTION_TIMER_HPP
#define MICROLUTION_TIMER_HPP

#include "raylib.h"

class Timer
{
public:
	explicit Timer(float delayInSeconds = 0) :
			mDelay(delayInSeconds) {}

	void Update(float delta)
	{
		mTime += delta;
	};

	bool IsElapsed() const { return mTime >= mDelay; }

	void Reset() { mTime = 0; }

	float GetTime() const { return mTime; }

	void SetDelay(float newDelay) { mDelay = newDelay; }

private:
	float mDelay;
	float mTime = 0;
};

#endif //MICROLUTION_TIMER_HPP
