#pragma once


class Stopwatch
{
private:
	using clock = std::chrono::steady_clock;
	using instant = std::chrono::steadyclock::timepoint;

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