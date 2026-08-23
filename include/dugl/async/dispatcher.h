#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "dugl/common.h"

#include <functional>


DUGL_NAMESPACE_BEGIN

/// General-purpose entry point for submitting concurrent work, backed by a Jolt-compatible
/// job system so the same scheduler drives both physics simulation and arbitrary async work.
class AsyncDispatcher
{
private:
    JPH::JobSystemThreadPool jobSystem;

public:
    AsyncDispatcher(uint32 maxJobs, uint32 maxBarriers, int numThreads = -1);

    /// Execute the given job function concurrently.
    void submitJob(const char* name, std::function<void()> func, uint32 numDependencies = 0);

    /// Change the max concurrency after initialization.
    void setNumThreads(int numThreads);

    /// The underlying Jolt job system, e.g. for JPH::PhysicsSystem::Update().
    JPH::JobSystem& getJobSystem();
};

DUGL_NAMESPACE_END
