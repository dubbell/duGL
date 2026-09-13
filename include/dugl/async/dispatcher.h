#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "dugl/common.h"
#include "dugl/async/job.h"
#include "dugl/async/barrier.h"


DUGL_NAMESPACE_BEGIN

/* Entry point for submitting concurrent work, backed by a Jolt-compatible job system so
   the same scheduler drives both physics simulation and arbitrary async work. */
class AsyncDispatcher
{
private:
    JPH::JobSystemThreadPool jobSystem;

public:
    AsyncDispatcher(uint32 maxJobs, uint32 maxBarriers, int numThreads = -1);

    /* Execute the given job concurrently. The caller must keep the job alive until it has
       finished executing. Pass a barrier (see createBarrier()) to track completion via
       waitAndDestroyBarrier(), or nullptr to fire and forget. */
    void submitJob(const char* name, Job* job, JobBarrier* barrier = nullptr, uint32 numDependencies = 0);

    /* Create a barrier that jobs can be added to (via submitJob) to later wait on their completion. */
    JobBarrier* createBarrier();

    /* Block the calling thread until every job added to the barrier has finished executing,
       then destroy the barrier. */
    void waitAndDestroyBarrier(JobBarrier* barrier);

    /* Change the max concurrency after initialization. */
    void setNumThreads(int numThreads);

    /* Get the max concurrency. */
    int getNumThreads();
};

DUGL_NAMESPACE_END
