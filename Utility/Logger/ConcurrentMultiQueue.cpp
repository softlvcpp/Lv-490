#include "Other/pch.h"
#include "Essentials/ConcurrentMultiQueue.h"
#include <cassert>

namespace log490
{

	ConcurrentMultiQueue::ConcurrentMultiQueue() : waitCounter{ 0 }, fStopWaits{ false }
	{ }
	ConcurrentMultiQueue::ConcurrentMultiQueue(ConcurrentMultiQueue&& source) noexcept
	{
		this->operator=(std::move(source));
	}
	ConcurrentMultiQueue::~ConcurrentMultiQueue()
	{
		cancelWaits();
	}

	ConcurrentMultiQueue& ConcurrentMultiQueue::operator=(ConcurrentMultiQueue&& right) noexcept
	{
		if (this != &right)
		{
			std::scoped_lock lock{ this->writeMutex, right.writeMutex };
			this->threadOrderedSet = std::move(right.threadOrderedSet);
			this->threadQueues = std::move(right.threadQueues);
			this->overallSize = std::move(right.overallSize.load());
		}
		return *this;
	}

#ifdef _DEBUG
	bool ConcurrentMultiQueue::assertSize() const
	{
		size_t sizeSum = 0;
		for (auto& it : threadQueues)
			sizeSum += it.second.size();
		return (threadOrderedSet.size() == threadQueues.size()) && overallSize == sizeSum;
	}
#endif // _DEBUG

	/* Moves new value into the queue */
    void ConcurrentMultiQueue::emplace(LogData&& data)
	{
		std::scoped_lock lock{ writeMutex };
		if (threadQueues.find(data.threadID()) == threadQueues.end())
			threadQueues[data.threadID()] = {};
		threadQueues[data.threadID()].emplace(std::move(data));
		threadOrderedSet.emplace(threadQueueRef{ threadQueues[data.threadID()] });
		overallSize++;
		assert(assertSize());
	}

	/* Cancels blocking for any waiting threads */
	void ConcurrentMultiQueue::cancelWaits()
	{
		std::scoped_lock lock(writeMutex);
		if (waitCounter.load() != 0)
		{
			fStopWaits.store(true);
			popVar.notify_all();
		}
	}
	/* Clears  */
	void ConcurrentMultiQueue::clear()
	{
		std::scoped_lock lock(writeMutex);
		threadOrderedSet.clear();
		threadQueues.clear();
		overallSize.store(0);
		assert(assertSize());
	}

	/* Checks if queue is size() == 0 */
	bool ConcurrentMultiQueue::empty() const
	{
		return overallSize.load() == 0;
	}
	/* Current queue size */
	size_t ConcurrentMultiQueue::size() const
	{
		return overallSize;
	}

	/* Tries to pop front() if not empty and assigns through moving */
	bool ConcurrentMultiQueue::tryPop(LogData& out)
	{
		std::scoped_lock lock(writeMutex);
		if (overallSize == 0)
			return false;

		popItem(out);
		return true;
	}
	/* Waits to pop front() until there is at least one item in the queue.
	   Assigns through moving.
	   Returns false if wait was canceled
	*/
	bool ConcurrentMultiQueue::waitPop(LogData& out)
	{
		std::unique_lock lock(writeMutex);
		if (!wait(lock))
			return false;

		popItem(out);
		return true;
	}

	inline bool ConcurrentMultiQueue::wait(std::unique_lock<std::mutex>& lock)
	{
		if (empty())
		{
			waitCounter++;
			popVar.wait(lock, [this]() { return !empty() || fStopWaits.load(); });
			waitCounter--;
			if (fStopWaits.load())
			{
				if (waitCounter.load() == 0)
					fStopWaits.store(false);
				return false;
			}
		}
		return true;
	}
	inline void ConcurrentMultiQueue::popItem(LogData& out)
	{
		auto& topQueue = threadOrderedSet.begin()->get();

		out = std::move(topQueue.front());
		topQueue.pop();
		if (threadOrderedSet.size() == 1)
		{
			if (topQueue.empty())
			{
				threadQueues.erase(threadQueues.find(out.threadID()));
				threadOrderedSet.erase(threadOrderedSet.begin());
			}
		}
		else
		{
			threadOrderedSet.erase(threadOrderedSet.begin());
			if (topQueue.empty())
				threadQueues.erase(threadQueues.find(out.threadID()));
			else
				threadOrderedSet.emplace(threadQueueRef{ topQueue });
		}
		overallSize--;
		assert(assertSize());
	}


	bool LogDataMQLess::operator()(const LogData& leftData, const LogData& rightData) const
	{
		if (leftData.threadID() == rightData.threadID())
			return false;
		else if (leftData.msgHighResTime() == rightData.msgHighResTime())
			return leftData.threadID() < rightData.threadID();
		else
			return leftData.msgHighResTime() < rightData.msgHighResTime();
	}

	bool LogDataMQLess::operator()(const std::queue<LogData>& left, const std::queue<LogData>& right) const
	{
		return (*this)(left.front(), right.front());
	}

}
