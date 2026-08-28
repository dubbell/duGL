#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>


// Initialize Jolt for physics simulation.
inline void initJolt()
{
	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();
}