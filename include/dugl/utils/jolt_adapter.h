#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>


inline void initJolt()
{
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();
}