#pragma once

#include <unordered_map>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include "Essentials/LogFundamentals.h"

namespace log490
{

	struct LogDataMQLess
	{
		LOGGER_API bool operator()(const LogData& left, const LogData& right) const;
		LOGGER_API bool operator()(const std::queue<LogData>& left, const std::queue<LogData>& right) const;
	};

	class ConcurrentMultiQueue
	{
	public:
	    LOGGER_API ConcurrentMultiQueue();
		LOGGER_API ConcurrentMultiQueue(ConcurrentMultiQueue&& source) noexcept;
		LOGGER_API ~ConcurrentMultiQueue();

		LOGGER_API ConcurrentMultiQueue& operator=(ConcurrentMultiQueue&& right) noexcept;
		
		/* Moves new value into the queue */
		LOGGER_API void emplace(LogData&& data);
		
		/* Cancels blocking for any waiting threads */
		LOGGER_API void cancelWaits();
		/* Clears queue */
		LOGGER_API void clear();

		/* Checks if queue is size() == 0 */
		LOGGER_API bool empty() const;
		/* Current queue size */
		LOGGER_API size_t size() const;
		/* Tries to pop front() if not empty and assigns through moving */
		LOGGER_API bool tryPop(LogData& out);
		/* Waits to pop front() until there is at least one item in the queue.
		   Assigns through moving.
		   Returns false if wait was canceled
		*/
		LOGGER_API bool waitPop(LogData& out);
	private:
#ifdef _DEBUG
		bool assertSize() const;
#endif // _DEBUG

		bool wait(std::unique_lock<std::mutex>& lock);
		void popItem(LogData& out);
	private:
		using queueType = std::queue<LogData>;
		using threadQueueMap = std::unordered_map<std::thread::id, queueType>;
		using threadQueueRef = std::reference_wrapper<queueType>;

		std::set<threadQueueRef, LogDataMQLess> threadOrderedSet;		// Ordered set that sorts threadQueues top items by time
		std::unordered_map<std::thread::id, queueType> threadQueues;    // Container of queues that maps them by id of thread that emplaced all of their items

		std::atomic<size_t> overallSize;

		std::atomic<bool> fStopWaits;		
		std::atomic<size_t> waitCounter;
		mutable std::mutex writeMutex;
		std::condition_variable popVar;
	};

}

