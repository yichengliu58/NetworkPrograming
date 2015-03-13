#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <memory>
#include <thread>
#include <iostream>
#include <functional>
#include <string>
#include <random>

template<typename T>
class Buffer
{
public:
	Buffer(std::size_t size)
	:size(size),queue(),lock(),cond()
	{
	}
	Buffer(const Buffer& other)
	:size(other.size),queue(other.queue),lock(),cond()
	{
	}
	~Buffer() = default;
	Buffer& operator=(const Buffer&) = delete;

	void Push(const T&);
	std::shared_ptr<T> Pop();
	bool Empty() const;
private:
	std::size_t size;
	std::deque<T> queue;
	mutable std::mutex lock;
	std::condition_variable cond;
};

template<typename T>
void Buffer<T>::Push(const T& production)
{
	std::unique_lock<std::mutex> guard(lock);
	while(queue.size() == size)
	{
		std::cout << "push waiting " << std::endl;
		cond.wait(guard);
	}
	queue.push_back(production);
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	cond.notify_one();
}

template<typename T>
std::shared_ptr<T> Buffer<T>::Pop()
{
	std::unique_lock<std::mutex> guard(lock);
	while(queue.empty())
	{
		std::cout << "pop waiting" << std::endl;
		cond.wait(guard);
	}
	std::shared_ptr<T> ptr = std::make_shared<T>(queue.front());
	queue.pop_front();
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	cond.notify_one();
	return ptr;
}

template<typename T>
bool Buffer<T>::Empty() const
{
	std::unique_lock<std::mutex> guard(lock);
	return queue.empty();
}

template<typename T>
void Producer(Buffer<T>& buf,std::function<T()> fun)
{
	for(int i = 0;i < 10000;++i)
	{	
		buf.Push(fun());
	}
}

template<typename T>
void Consumer(Buffer<T>& buf)
{
	for(int i = 0;i < 10000;++i)
	{
		std::cout << *(buf.Pop()) << std::endl;
	}
}

int getInt()
{
	//generate random numbers from 0 to 1000
	static std::uniform_int_distribution<int> range(0,1000);
	//get default random engine
	static std::default_random_engine engine;
	return range(engine);
}

int main()
{
	Buffer<int> buffer(100);
	std::thread produce(Producer<int>,std::ref(buffer),getInt);
	std::thread consume(Consumer<int>,std::ref(buffer));
	produce.join();
	consume.join();
	return 0;
}

