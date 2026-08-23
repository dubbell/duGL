#include "dugl/async/dispatcher.h"

using namespace dugl;


AsyncDispatcher::AsyncDispatcher(uint32 maxJobs, uint32 maxBarriers, int numThreads)
    : jobSystem(maxJobs, maxBarriers, numThreads)
{}

void AsyncDispatcher::submitJob(const char* name, std::function<void()> func, uint32 numDependencies)
{
    jobSystem.CreateJob(name, JPH::Color::sRed, func, numDependencies);
}

void AsyncDispatcher::setNumThreads(int numThreads)
{
    jobSystem.SetNumThreads(numThreads);
}

JPH::JobSystem& AsyncDispatcher::getJobSystem()
{
    return jobSystem;
}
