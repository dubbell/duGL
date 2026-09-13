#include "dugl/async/dispatcher.h"

using namespace dugl;

namespace {

// Wraps the underlying Jolt barrier so JobBarrier consumers never see Jolt types.
class JoltJobBarrier : public JobBarrier
{
public:
    JPH::JobSystem::Barrier* barrier;
    explicit JoltJobBarrier(JPH::JobSystem::Barrier* barrier) : barrier(barrier) {}
};

}

AsyncDispatcher::AsyncDispatcher(uint32 maxJobs, uint32 maxBarriers, int numThreads)
    : jobSystem(maxJobs, maxBarriers, numThreads)
{}

void AsyncDispatcher::submitJob(const char* name, Job* job, JobBarrier* barrier, uint32 numDependencies)
{
    JPH::JobSystem::JobHandle handle = jobSystem.CreateJob(name, JPH::Color::sRed, [job]() { job->execute(); }, numDependencies);
    if (barrier != nullptr) {
        static_cast<JoltJobBarrier*>(barrier)->barrier->AddJob(handle);
    }
}

JobBarrier* AsyncDispatcher::createBarrier()
{
    return new JoltJobBarrier(jobSystem.CreateBarrier());
}

void AsyncDispatcher::waitAndDestroyBarrier(JobBarrier* barrier)
{
    JoltJobBarrier* joltBarrier = static_cast<JoltJobBarrier*>(barrier);
    jobSystem.WaitForJobs(joltBarrier->barrier);
    jobSystem.DestroyBarrier(joltBarrier->barrier);
    delete joltBarrier;
}

void AsyncDispatcher::setNumThreads(int numThreads)
{
    jobSystem.SetNumThreads(numThreads);
}

int AsyncDispatcher::getNumThreads()
{
    return jobSystem.GetMaxConcurrency();
}