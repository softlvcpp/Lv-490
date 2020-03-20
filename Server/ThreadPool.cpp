#include "ThreadPool.h"

void ThreadPool::StartPool(std::size_t numThreads)
{
	for (size_t i = 0; i < numThreads; ++i)
	{
		m_threads.emplace_back([=]
		{
			while (true)
			{
				Task task;

				{//this scope is for synchronization
					std::unique_lock<std::mutex> lock{ m_event_mutex };

					m_event_var.wait(lock, [=] { return m_stopping || !m_tasks.empty(); });

					if (m_stopping && m_tasks.empty())//for synchronization is needed to have 'while(true)' and break loop
						break;

					//get first task from queue and delete it
					task = std::move(m_tasks.front());
					m_tasks.pop();
				}

				task();//execute task												
			}
		});
	}
}

void ThreadPool::StopPool() noexcept
{
	{//this scope is for synchronization
		std::unique_lock<std::mutex> lock{ m_event_mutex };
		m_stopping = true;
	}

	m_event_var.notify_all();//unblock threads

	for (auto& thread : m_threads)
	{
		thread.join();
	}
}