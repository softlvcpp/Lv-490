#pragma once




class ThreadPool
{
public:
	using Task = std::function<void()>;//function wrapper

	explicit ThreadPool(std::size_t num_threads) { StartPool(num_threads); }

	~ThreadPool() { StopPool(); }

	template<typename T, typename ...Args>
	auto ExecuteTask(T&& task, Args&& ...args)->std::future<typename std::result_of<T(Args...)>::type>;

private:
	std::vector<std::thread> m_threads;

	std::condition_variable m_event_var;

	std::mutex m_event_mutex;
	bool m_stopping = false;

	std::queue<Task> m_tasks;//queue with tasks

	void StartPool(std::size_t numThreads);//start pool work

	void StopPool() noexcept;//stop pool work
};

template<typename T, typename ...Args>
inline auto ThreadPool::ExecuteTask(T&& task, Args&& ...args) -> std::future<typename std::result_of<T(Args ...)>::type>
{
	using return_type = typename std::result_of<T(Args...)>::type;

	auto wrapper = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<T>(task), std::forward<Args>(args)...));//initialize wrapper for input function

	{//this scope is for synchronization
		std::unique_lock<std::mutex> lock{ m_event_mutex };
		m_tasks.emplace([wrapper] {
			(*wrapper)();
		});
	}

	m_event_var.notify_one();
	return wrapper->get_future();
}