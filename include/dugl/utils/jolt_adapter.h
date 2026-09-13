#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>


// Initialize Jolt for physics simulation and for the job system backing AsyncDispatcher.
inline void initJolt()
{
	static bool initialized = false;
	if (initialized) {
		return;
	}
	initialized = true;

	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();
}