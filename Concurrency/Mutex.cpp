#include <pthread.h>
#include <unistd.h>
#include <iostream>
using namespace std;
enum MutexType{Normal,ErrorCheck,Recursive};

class Mutex
{
	friend class Locker;
public:
	Mutex(MutexType type);
	~Mutex();
	Mutex(const Mutex&) = delete;
private:
	bool Lock()
	{
		return pthread_mutex_lock(&mutex) == 0;
	}
	bool Unlock()
	{
		return pthread_mutex_unlock(&mutex) == 0;
	}
	
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
};

class Locker
{
public:
	explicit Locker(Mutex& m)
		:mutex(m)
	{
		mutex.Lock();
	}
	~Locker()
	{
		mutex.Unlock();
	}
private:
	Mutex& mutex;
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

Mutex mtx(MutexType::Normal);

void thread()
{
	//Locker lock(mtx);
	for(int i = 0;i < 3;i++)
		cout << 3 << endl;
	pthread_exit(nullptr);
}

void thread2()
{
	//Locker lock(mtx);
	for(int i = 0;i < 3;i++)
	{
		cout << "fuck" << endl;
		sleep(1);
	}
	pthread_exit(nullptr);
} 

int main()
{
	pthread_t t1;
	pthread_t t2;
	pthread_create(&t1,nullptr,(void*(*)(void*))thread,nullptr);
	pthread_create(&t2,nullptr,(void*(*)(void*))thread2,nullptr);
	pthread_join(t2,nullptr);
	pthread_join(t1,nullptr);
	return 0;
}