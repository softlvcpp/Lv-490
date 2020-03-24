#pragma once
#ifndef CONCPRIORITYQUEUE_H
#define CONCPRIORITYQUEUE_H

#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <time.h>

namespace log490
{

	template<typename TData, typename TContainer = std::vector<TData>, typename TComparer = std::less<typename TContainer::value_type>>
	class ConcurrentPriorityQueue : protected std::priority_queue<TData, TContainer, TComparer>
	{
	public:
		using base_q = std::priority_queue<TData, TContainer, TComparer>;
		using queue_type = ConcurrentPriorityQueue<TData, TContainer, TComparer>;

		ConcurrentPriorityQueue();
		ConcurrentPriorityQueue(ConcurrentPriorityQueue&&);
		ConcurrentPriorityQueue(const ConcurrentPriorityQueue&);
		~ConcurrentPriorityQueue();

		ConcurrentPriorityQueue& operator=(const ConcurrentPriorityQueue&);
		ConcurrentPriorityQueue& operator=(ConcurrentPriorityQueue&&);

		void emplace(TData&& data);
		void push(const TData& data);

		void cancelWaits();
        void clear();

		bool empty() const;
		size_t size() const;

		bool tryPop(TData& out);
		bool tryPopMove(TData& out);
		bool waitPop(TData& out);
		bool waitPopMove(TData& out);
	private:
		bool wait(std::unique_lock<std::mutex>& lock);
	private:
		std::atomic<bool> fStopWaits;
		std::atomic<size_t> waitCounter;
		mutable std::mutex writeMutex;
		std::condition_variable popVar;
	};

	template<typename TData, typename TContainer, typename TComparer>
	inline ConcurrentPriorityQueue<TData, TContainer, TComparer>::ConcurrentPriorityQueue()
		: waitCounter{ 0 }, fStopWaits{ false }, base_q()
	{ }

	template<typename TData, typename TContainer, typename TComparer>
	inline ConcurrentPriorityQueue<TData, TContainer, TComparer>::ConcurrentPriorityQueue(ConcurrentPriorityQueue&& source)
		: queue_type()
	{
		this->operator=(std::move(source));
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline ConcurrentPriorityQueue<TData, TContainer, TComparer>::ConcurrentPriorityQueue(const ConcurrentPriorityQueue& source)
		: queue_type()
	{
		this->operator=(source);
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline ConcurrentPriorityQueue<TData, TContainer, TComparer>::~ConcurrentPriorityQueue()
	{
		cancelWaits();
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline ConcurrentPriorityQueue<TData, TContainer, TComparer>& ConcurrentPriorityQueue<TData, TContainer, TComparer>::operator=(const ConcurrentPriorityQueue& right)
	{
		if (this != &right)
		{
			std::scoped_lock lock{ this->writeMutex, right.writeMutex };
			base_q::operator=(static_cast<base_q&>(right));
		}
		return *this;
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline ConcurrentPriorityQueue<TData, TContainer, TComparer>& ConcurrentPriorityQueue<TData, TContainer, TComparer>::operator=(ConcurrentPriorityQueue&& right)
	{
		if (this != &right)
		{
			std::scoped_lock lock{ this->writeMutex, right.writeMutex };
			base_q::operator=(std::move(static_cast<base_q&>(right)));
		}
		return *this;
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline void ConcurrentPriorityQueue<TData, TContainer, TComparer>::emplace(TData&& data)
	{
		std::scoped_lock lock(writeMutex);
		base_q::emplace(std::move(data));
		popVar.notify_all();
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline void ConcurrentPriorityQueue<TData, TContainer, TComparer>::push(const TData& data)
	{
		std::scoped_lock lock(writeMutex);
		base_q::push(data);
		popVar.notify_all();
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline void ConcurrentPriorityQueue<TData, TContainer, TComparer>::cancelWaits()
	{
		std::scoped_lock lock(writeMutex);
		if (waitCounter.load() != 0)
		{
			fStopWaits.store(true);
			popVar.notify_all();
        }
    }

    template<typename TData, typename TContainer, typename TComparer>
    void ConcurrentPriorityQueue<TData, TContainer, TComparer>::clear()
    {
        std::scoped_lock lock(writeMutex);
        base_q::c.clear();
    }

	template<typename TData, typename TContainer, typename TComparer>
	inline size_t ConcurrentPriorityQueue<TData, TContainer, TComparer>::size() const
	{
		std::scoped_lock lock(writeMutex);
		return base_q::size();
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline bool ConcurrentPriorityQueue<TData, TContainer, TComparer>::empty() const
	{
		std::scoped_lock lock(writeMutex);
		return base_q::empty();
	}

	template<typename TData, typename TContainer, typename TComparer>
	[[nodiscard]]
	inline bool ConcurrentPriorityQueue<TData, TContainer, TComparer>::tryPop(TData& out)
	{
		std::scoped_lock lock(writeMutex);
		if (base_q::empty())
			return false;
		out = base_q::top();
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer, typename TComparer>
	[[nodiscard]]
	inline bool ConcurrentPriorityQueue<TData, TContainer, TComparer>::tryPopMove(TData& out)
	{
		std::scoped_lock lock(writeMutex);
		if (base_q::empty())
			return false;
		out = std::move(this->c.front());
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer, typename TComparer>
	[[nodiscard]]
	inline bool ConcurrentPriorityQueue<TData, TContainer, TComparer>::waitPop(TData& out)
	{
		std::unique_lock lock(writeMutex);
		if (!wait(lock))
			return false;
		out = base_q::top();
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer, typename TComparer>
	[[nodiscard]]
	inline bool ConcurrentPriorityQueue<TData, TContainer, TComparer>::waitPopMove(TData& out)
	{	
		std::unique_lock lock(writeMutex);
		if (!wait(lock))
			return false;
		out = std::move(base_q::c.front());
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer, typename TComparer>
	inline bool ConcurrentPriorityQueue<TData, TContainer, TComparer>::wait(std::unique_lock<std::mutex>& lock)
	{
		if (base_q::empty())
		{
			waitCounter++;
			popVar.wait(lock, [this]() { return !base_q::empty() || fStopWaits.load(); });
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



}
#endif // !1
