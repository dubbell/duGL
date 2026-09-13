#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


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

inline glm::vec3 toGlm(JPH::Vec3 v) { return { v.GetX(), v.GetY(), v.GetZ() }; }
inline glm::quat toGlm(JPH::Quat q) { return { q.GetW(), q.GetX(), q.GetY(), q.GetZ() }; }
inline JPH::Vec3 toJolt(glm::vec3 v) { return { v.x, v.y, v.z }; }
inline JPH::Quat toJolt(glm::quat q) { return { q.x, q.y, q.z, q.w }; }