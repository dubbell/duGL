#include "dugl/async/dispatcher.h"

#include <iostream>

using namespace dugl;
using namespace std;


AsyncDispatcher::AsyncDispatcher(uint maxJobs, uint maxBarriers, int numThreads)
{
	jobList.Init(maxJobs, maxJobs);
	for (std::atomic<Job*>& job : queue) {
		job = nullptr;
	}
	StartThreads(numThreads);
}

AsyncDispatcher::~AsyncDispatcher()
{
	StopThreads();
}

AsyncDispatcher::submitJob(const char* name, function<void()> func, uint32 numDependencies = 0)
{
	CreateJob(const char* name, JPH::Color::sRed, numDependencies);
}

// Below are the internal workings of the AsyncDispatcher, implementing
// the interface inherited from the JoltPhysics JobSystem.

void AsyncDispatcher::StartThreads(int numThreads)
{
	if (numThreads < 0) {
		numThreads = std::thread::hardware_concurrency() - 1;
	}
	if (numThreads == 0) {
		return;
	}

	quit = false;
	heads = reinterpret_cast<atomic<uint>*>(JPH::Allocate(sizeof(atomic<uint>) * numThreads));
	for (int i = 0; i < numThreads; i++) {
		heads[i] = 0;
	}

	threads.reserve(numThreads);
	for (int i = 0; i < numThreads; i++) {
		threads.emplace_back([this, i] { ThreadMain(i); });
	}
}

void AsyncDispatcher::StopThreads()
{
	if (threads.empty()) {
		return;
	}

	quit = true;
	semaphore.Release((uint)threads.size());

	for (thread& t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}

	threads.clear();

	for (uint head = 0; head != tail; head++) {
		Job* job = queue[head & (queueLength - 1)].exchange(nullptr);
		if (job != nullptr) {
			job->Execute();
			job->Release();
		}
	}

	JPH::Free(heads);
	heads = nullptr;
	tail = 0;
}

JPH::JobHandle AsyncDispatcher::CreateJob(const char* jobName, JPH::ColorArg color, const JobSystem::JobFunction& jobFunction, uint32 numDependencies)
{
	// Polling loop until a job has been constructed in the job list.
	uint32 index;
	for (;;) {
		index = jobList.ConstructObject(jobName, color, this, jobFunction, numDependencies);
		if (index != JPH::FixedSizeFreeList<Job>::cInvalidObjectIndex) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
	Job* job = &jobList.Get(index);

	JobHandle handle(job);

	if (numDependencies == 0) {
		QueueJob(job);
	}

	return handle;
}

void AsyncDispatcher::FreeJob(Job* job)
{
	jobList.DestructObject(job);
}

uint AsyncDispatcher::GetHead() const
{
	uint head = tail;
	for (size_t i = 0; i < threads.size(); i++) {
		head = min(head, heads[i].load());
	}
	return head;
}

void AsyncDispatcher::QueueJob(Job* job)
{
	if (threads.empty()) {
		return;
	}
	doQueueJob(job);
	semaphore.Release();
}

void AsyncDispatcher::QueueJobs(Job** jobs, uint numJobs)
{
	if (threads.empty()) {
		return;
	}

	for (Job** job = jobs; job < jobs + numJobs; job++) {
		doQueueJob(*job);
	}

	semaphore.Release(min(numJobs, (uint)threads.size()));
}

void AsyncDispatcher::doQueueJob(Job* job)
{
	job->AddRef();
	uint head = GetHead();

	for (;;)
	{
		// Poll until there is space in the queue.
		uint old_value = tail;
		if (old_value - head >= queueLength) {
			// Update head and perform check again.
			head = GetHead();
			old_value = tail;
			if (old_value - head >= queueLength) {
				// Wake up all threads to make them clear any nullptrs they may not have processed yet.
				semaphore.Release((uint)threads.size());
				// Sleep to wait for other threads to update their head pointers.
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				continue;
			}
		}

		// Write the job pointer if the slot is empty.
		Job* expected_job = nullptr;
		bool success = queue[old_value & (queueLength - 1)].compare_exchange_strong(expected_job, job);

		// Update tail value.
		tail.compare_exchange_strong(old_value, old_value + 1);

		// If the job was added successfully.
		if (success) {
			break;
		}
	}
}

void AsyncDispatcher::ThreadMain(int threadIndex)
{
	// Get the head for the thread.
	atomic<uint>& head = heads[threadIndex];
	while (!quit)
	{
		// Wait for jobs to be available.
		semaphore.Acquire();
		// This check should never fail since the semaphore can only be acquired if a 
		// job has been written to the queue at or before the tail.
		while (head != tail) {
			// Loop through the queue to find a job (not nullptr).
			atomic<Job*>& job = queue[head & (queueLength - 1)];
			if (job.load() != nullptr) {
				// Retrieve the job while emptying the queue slot.
				Job* job_ptr = job.exchange(nullptr);
				if (job_ptr != nullptr) {
					// Execute the job.
					job_ptr->Execute();
					job_ptr->Release();
				}
			}
			head++;
		}
	}
}

