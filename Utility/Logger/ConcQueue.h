#pragma once
#include <queue>
#include <atomic>
#include <condition_variable>


namespace log490
{

	class OrderedLock
	{
	public:
		OrderedLock() : isLocked(false) {};
		void lock() {
			std::unique_lock<std::mutex> acquire(accessMutex);
			if (isLocked) {
				varQueue.emplace();
				auto& varRef = varQueue.back();
				varQueue.back().wait(acquire, [this, &varRef]() { return &varRef == &varQueue.front(); });
			}
			else {
				isLocked = true;
			}
		}
		void unlock() {
			std::unique_lock<std::mutex> acquire(accessMutex);
			if (varQueue.empty()) {
				isLocked = false;
			}
			else {
				varQueue.front().notify_one();
				varQueue.pop();
			}
		}
		std::mutex& getMutex() { return accessMutex; }
	private:
		std::queue<std::condition_variable>  varQueue;
		std::mutex                           accessMutex;
		bool                                 isLocked;
	};

	class RAIIOrderedLock
	{
	public:
		RAIIOrderedLock(OrderedLock& lck)
			: lckRef{ lck }
		{
			lckRef.lock();
		}
		~RAIIOrderedLock()
		{
			lckRef.unlock();
		}
	private:
		OrderedLock& lckRef;
	};


	template<typename TData, typename TContainer = std::deque<TData>>
	class ConcurrentQueue : std::queue<TData, TContainer>
	{
	public:
		using base_q = std::queue<TData, TContainer>;
		using queue_type = ConcurrentQueue<TData, TContainer>;
		using lockable_object = OrderedLock;
		using lock_object = RAIIOrderedLock;

		ConcurrentQueue();
		ConcurrentQueue(ConcurrentQueue&&) noexcept;
		ConcurrentQueue(const ConcurrentQueue&);
		~ConcurrentQueue();

		ConcurrentQueue& operator=(const ConcurrentQueue&);
		ConcurrentQueue& operator=(ConcurrentQueue&&) noexcept;

		/* Moves new value into the queue */
		void emplace(TData&& data);
		/* Copies new value into the queue */
		void push(const TData& data);

		/* Cancels blocking for any waiting threads */
		void cancelWaits();
		/* Clears  */
		void clear();

		/* Checks if queue is size() == 0 */
		bool empty() const;
		/* Current queue size */
		size_t size() const;

		/* Tries to pop front() if not empty and assigns through copying */
		bool tryPop(TData& out);
		/* Tries to pop front() if not empty and assigns through moving */
		bool tryPopMove(TData& out);
		/* Waits to pop front() until there is at least one item in the queue.
		   Assigns through copying.
		   Returns false if wait was canceled
		*/
		bool waitPop(TData& out);
		/* Waits to pop front() until there is at least one item in the queue.
		   Assigns through moving.
		   Returns false if wait was canceled
		*/
		bool waitPopMove(TData& out);
	private:
		bool wait(std::unique_lock<std::mutex>& lock);

		void copyToThis(const ConcurrentQueue& queue);
		void moveToThis(ConcurrentQueue&& queue);
	private:
		std::atomic<bool> fStopWaits;
		std::atomic<size_t> waitCounter;
		//mutable std::mutex writeMutex;
		mutable OrderedLock writeLockable;
		std::condition_variable popVar;
	};

	template<typename TData, typename TContainer>
	inline ConcurrentQueue<TData, TContainer>::ConcurrentQueue()
		: waitCounter{ 0 }, fStopWaits{ false }, base_q()
	{ }

	template<typename TData, typename TContainer>
	inline ConcurrentQueue<TData, TContainer>::ConcurrentQueue(ConcurrentQueue&& source) noexcept
		: queue_type()
	{
		moveToThis(std::move(source));
	}

	template<typename TData, typename TContainer>
	inline ConcurrentQueue<TData, TContainer>::ConcurrentQueue(const ConcurrentQueue& source)
		: queue_type()
	{
		copyToThis(source);
	}

	template<typename TData, typename TContainer>
	inline ConcurrentQueue<TData, TContainer>::~ConcurrentQueue()
	{
		cancelWaits();
	}

	template<typename TData, typename TContainer>
	void ConcurrentQueue<TData, TContainer>::copyToThis(const ConcurrentQueue& source)
	{
		//std::scoped_lock lock{ this->writeMutex, right.writeMutex };				
		lock_object tlck{ this->writeLockable };
		lock_object rlck{ source.writeLockable };
		base_q::operator=(static_cast<base_q&>(source));
	}

	template<typename TData, typename TContainer>
	void ConcurrentQueue<TData, TContainer>::moveToThis(ConcurrentQueue&& source)
	{
		//std::scoped_lock lock{ this->writeMutex, right.writeMutex };				
		lock_object tlck{ this->writeLockable };
		lock_object rlck{ source.writeLockable };
		base_q::operator=(std::move(static_cast<base_q&>(source)));
	}

	template<typename TData, typename TContainer>
	inline ConcurrentQueue<TData, TContainer>& ConcurrentQueue<TData, TContainer>::operator=(const ConcurrentQueue& right)
	{
		if (this != &right)
			copyToThis(right);
		return *this;
	}

	template<typename TData, typename TContainer>
	inline ConcurrentQueue<TData, TContainer>& ConcurrentQueue<TData, TContainer>::operator=(ConcurrentQueue&& right) noexcept
	{
		if (this != &right)
			moveToThis(std::move(right));
		return *this;
	}

	template<typename TData, typename TContainer>
	inline void ConcurrentQueue<TData, TContainer>::emplace(TData&& data)
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		base_q::emplace(std::move(data));
		popVar.notify_all();
	}

	template<typename TData, typename TContainer>
	inline void ConcurrentQueue<TData, TContainer>::push(const TData& data)
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		base_q::push(data);
		popVar.notify_all();
	}

	template<typename TData, typename TContainer>
	inline void ConcurrentQueue<TData, TContainer>::cancelWaits()
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		if (waitCounter.load() != 0)
		{
			fStopWaits.store(true);
			popVar.notify_all();
		}
	}

	template<typename TData, typename TContainer>
	void ConcurrentQueue<TData, TContainer>::clear()
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		base_q::c.clear();
	}

	template<typename TData, typename TContainer>
	inline size_t ConcurrentQueue<TData, TContainer>::size() const
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		return base_q::size();
	}

	template<typename TData, typename TContainer>
	inline bool ConcurrentQueue<TData, TContainer>::empty() const
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		return base_q::empty();
	}

	template<typename TData, typename TContainer>
	[[nodiscard]]
	inline bool ConcurrentQueue<TData, TContainer>::tryPop(TData& out)
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		if (base_q::empty())
			return false;
		out = base_q::front();
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer>
	[[nodiscard]]
	inline bool ConcurrentQueue<TData, TContainer>::tryPopMove(TData& out)
	{
		//std::scoped_lock lock(writeMutex);
		lock_object lock{ writeLockable };
		if (base_q::empty())
			return false;
		//out = std::move(this->c.front());
		out = std::move(const_cast<TData&>(base_q::front()));
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer>
	[[nodiscard]]
	inline bool ConcurrentQueue<TData, TContainer>::waitPop(TData& out)
	{
		//std::unique_lock lock(writeMutex);
		std::unique_lock lock{ writeLockable.getMutex() };
		if (!wait(lock))
			return false;
		out = base_q::front();
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer>
	[[nodiscard]]
	inline bool ConcurrentQueue<TData, TContainer>::waitPopMove(TData& out)
	{
		//std::unique_lock lock(writeMutex);
		std::unique_lock lock{ writeLockable.getMutex() };
		if (!wait(lock))
			return false;
		//out = std::move(base_q::c.front());
		out = std::move(const_cast<TData&>(base_q::front()));
		base_q::pop();
		return true;
	}

	template<typename TData, typename TContainer>
	inline bool ConcurrentQueue<TData, TContainer>::wait(std::unique_lock<std::mutex>& lock)
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
