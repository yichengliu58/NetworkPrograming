#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <memory>
#include <thread>
#include <iostream>
#include <functional>

template<typename T>
class Buffer
{
public:
	Buffer(std::size_t size)
	:size(size),queue(),lock(),cond()
	{
	}
	Buffer(const Buffer&) = delete;
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

void Producer(Buffer<double>& buf)
{
	int i;
	for(i = 1;i < 10000000;++i)
	{	
		buf.Push(static_cast<double>(i)/5);
	}
}

void Consumer(Buffer<double>& buf)
{
	int i;
	for(i = 1;i < 10000000;++i)
	{
		std::cout << *(buf.Pop()) << std::endl;
	}
}

int main()
{
	Buffer<double> buffer(100);
	std::thread produce(Producer,std::ref(buffer));
	std::thread consume(Consumer,std::ref(buffer));
	produce.join();
	consume.join();
	return 0;
}

