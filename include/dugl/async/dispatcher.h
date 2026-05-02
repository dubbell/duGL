#pragma once

#include "dugl/common.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemWithBarrier.h>
#include <Jolt/Core/FixedSizeFreeList.h>
#include <Jolt/Core/Semaphore.h>


DUGL_NAMESPACE_BEGIN

/// Thread pool for managing threads performing tasks that can be executed concurrently.
class AsyncDispatcher : public JPH::JobSystemWithBarrier
{
private:
	static constexpr JPH::uint32 queueLength = 1024;      /// Length of the job queue.
	 
	JPH::Array<std::thread> threads;                      /// Worker threads.
	JPH::FixedSizeFreeList<JPH::JobSystem::Job> jobList;  /// Job list.

	std::atomic<JPH::JobSystem::Job*> queue[queueLength]; /// The job queue, a lock-free ring buffer.

	// Note that these are computed modulo queueLength-1 when used to access the queue.
	std::atomic<uint>* heads = nullptr;                   /// Next position in the queue from which a job will be read for each thread.
	std::atomic<uint> tail = 0;                           /// Next position in the queue to write a job.

	JPH::Semaphore semaphore;                             /// Used to signal worker threads that there is more work.
	std::atomic<bool> quit = false;                       /// Indicates that the job system should stop execution.

public:
	JPH_OVERRIDE_NEW_DELETE
	AsyncDispatcher(uint maxJobs, uint maxBarriers, int numThreads = -1);
	virtual ~AsyncDispatcher() override;

	void submitJob(const char* name, function<void()> func, uint32 numDependencies = 0);
	
	/// Change the max concurrency after initialization.
	void setNumThreads(int numThreads) { StopThreads(); StartThreads(numThreads); };

protected:
	/// Create a new job, starts immediately if it has no dependencies.
	JPH::JobHandle CreateJob(const char* name, 
							 JPH::ColorArg color, 
							 const JPH::JobSystem::JobFunction& jobFunction, 
							 uint32 numDependencies = 0) override;

	void QueueJob(JPH::JobSystem::Job* job) override;
	void QueueJobs(JPH::JobSystem::Job** jobs, uint numJobs) override;
	void FreeJob(JPH::JobSystem::Job* job) override;

	/// Maximum number of concurrently executing jobs.
	int GetMaxConcurrency() const override;

private:
	/// Starts job execution by worker threads.
	void StartThreads(int numThreads);
	/// Stops job execution by worker threads.
	void StopThreads();

	/// Thread entry point.
	void ThreadMain(int threadIndex);

	/// The head of the thread that has processed the least amount of jobs.
	inline uint GetHead() const;

	inline void doQueueJob(JPH::JobSystem::Job* job);
};

DUGL_NAMESPACE_END