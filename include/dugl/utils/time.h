#pragma once

#include <chrono>


// Used to keep track of time between calls to tick().
class Stopwatch
{
private:
	using clock = std::chrono::steady_clock;
	using instant = clock::time_point;

	instant lastTime;

public:
	Stopwatch() : lastTime(clock::now()) {}

	float tick()
	{
		auto now = clock::now();
		float dt = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;
		return dt;
	}
};