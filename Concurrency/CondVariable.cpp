#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;
enum MutexType{Normal,ErrorCheck,Recursive};

class Mutex
{
	friend class Locker;
public:
	Mutex(MutexType type);
	~Mutex();
	Mutex(const Mutex&) = delete;
	pthread_mutex_t* GetMutex();
	bool Lock()
	{
		return pthread_mutex_lock(&mutex) == 0;
	}
	bool Unlock()
	{
		return pthread_mutex_unlock(&mutex) == 0;
	}
private:
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
};

Mutex::Mutex(MutexType type = MutexType::Normal)
{
	pthread_mutexattr_init(&attr);
	switch(type)
	{
		case Normal:
		{
			pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_NORMAL);
			pthread_mutex_init(&mutex,&attr);
		}
		case ErrorCheck:
		{
			pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
			pthread_mutex_init(&mutex,&attr);
		}
		case Recursive:
		{
			pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);				pthread_mutex_init(&mutex,&attr);
		}
	}
}

Mutex::~Mutex()
{
	pthread_mutexattr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
}

pthread_mutex_t* Mutex::GetMutex()
{
	return &mutex;
}

class CondVariable
{
public:
	explicit CondVariable(Mutex& m)
		:mutex(m)
	{
		pthread_cond_init(&cond,nullptr);
	}
	~CondVariable()
	{
		pthread_cond_destroy(&cond);
	}
	bool Wait()
	{
		int ret = pthread_cond_wait(&cond,mutex.GetMutex());
		return ret == 0;
	}
	bool Notify()
	{
		return pthread_cond_signal(&cond) == 0;
	}
	bool NotifyAll()
	{
		return pthread_cond_broadcast(&cond) == 0;
	}
private:
	Mutex& mutex;
	pthread_cond_t cond;
};

Mutex mtx(MutexType::Normal);
CondVariable condc(mtx);
CondVariable condp(mtx);
int buffer = 0;

void Producer()
{
	for(int i = 0;i < 10;i++)
	{
		mtx.Lock();
		while(buffer != 0)
			condp.Wait();
		buffer = 100;
		cout << "produce data" << buffer << endl;
		sleep(1);
		condc.Notify();
		mtx.Unlock();
	}
}

void Consumer()
{
	for(int i = 0;i < 10;i++)
	{
		mtx.Lock();
		while(buffer == 0)
			condc.Wait();
		buffer = 0;
		cout << "consume data " << buffer << endl;
		sleep(2);
		condp.Notify();
		mtx.Unlock();
	}
}

int main()
{
	pthread_t p_thread;
	pthread_t c_thread;
	pthread_create(&c_thread,nullptr,(void*(*)(void*))Consumer,nullptr);
	pthread_create(&p_thread,nullptr,(void*(*)(void*))Producer,nullptr);
	pthread_join(p_thread,0);
	pthread_join(c_thread,0);
	return 0;
}