#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemWithBarrier.h>
#include <Jolt/Core/FixedSizeFreeList.h>
#include <Jolt/Core/Semaphore.h>

#include "dugl/common.h"


DUGL_NAMESPACE_BEGIN

/// Manager for concurrent execution of jobs.
class AsyncDispatcher : public JPH::JobSystemWithBarrier
{
	template<typename T> using atomic = std::atomic<T>;
	using thread = std::thread;
	using Job = JPH::JobSystem::Job;
	using JobHandle = JPH::JobHandle;
	using JobFunction = JPH::JobSystem::JobFunction;
	using Semaphore = JPH::Semaphore;
	template<typename T> using Array = JPH::Array<T>;
	template<typename T> using FixedSizeFreeList = JPH::FixedSizeFreeList<T>;

private:
	static constexpr uint32 queueLength = 1024;      /// Length of the job queue.
	 
	Array<thread> threads;                           /// Worker threads.
	FixedSizeFreeList<Job> jobList;                  /// Job list.

	atomic<Job*> queue[queueLength];                 /// The job queue, a lock-free ring buffer.

	// Note that these are computed modulo queueLength-1 when used to access the queue.
	atomic<uint>* heads = nullptr;                   /// Next position in the queue from which a job will be read for each thread.
	atomic<uint> tail = 0;                           /// Next position in the queue to write a job.

	Semaphore semaphore;                             /// Used to signal worker threads that there is more work.
	atomic<bool> quit = false;                       /// Indicates that the job system should stop execution.

public:
	JPH_OVERRIDE_NEW_DELETE
	AsyncDispatcher(uint maxJobs, uint maxBarriers, int numThreads = -1);
	virtual ~AsyncDispatcher() override;

	/// Execute the given job function concurrently.
	void submitJob(const char* name, std::function<void()> func, uint32 numDependencies = 0);
	
	/// Change the max concurrency after initialization.
	void setNumThreads(int numThreads) { StopThreads(); StartThreads(numThreads); };

protected:
	/// Create a new job, starts immediately if it has no dependencies.
	JobHandle CreateJob(const char* name, 
						JPH::ColorArg color, 
						const JobFunction& jobFunction, 
						uint32 numDependencies = 0) override;

	/// Queue a job for execution.
	void QueueJob(Job* job) override;
	/// Queue multiple jobs for execution.
	void QueueJobs(Job** jobs, uint numJobs) override;
	/// Remove a job from the job list.
	void FreeJob(Job* job) override;

	/// Maximum number of concurrently executing jobs.
	int GetMaxConcurrency() const override { return threads.size() + 1; }

private:
	/// Starts job execution by worker threads.
	void StartThreads(int numThreads);
	/// Stops job execution by worker threads.
	void StopThreads();

	/// Thread entry point.
	void ThreadMain(int threadIndex);

	/// The head of the thread that has processed the least amount of jobs.
	inline uint GetHead() const;

	inline void doQueueJob(Job* job);
};

DUGL_NAMESPACE_END