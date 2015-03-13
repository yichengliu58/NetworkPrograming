#include <iostream>
template<typename T>
class SmartPtr
{
public:
	//default constructor
	SmartPtr()
	:point(nullptr),count(0)
	{}
	//constructor(1)
	SmartPtr(T* ptr)
	:point(ptr),count(1)
	{}
	//copy constructor
	SmartPtr(const SmartPtr& other)
	//:point(other.point),count(other.count+1)
	{
		if(&other != this)
		{
			point = other.point;
			count = ++other.count;
		}
	}
	//destructor
	~SmartPtr()
	{
		if(--count == 0)
			delete point;
	}
	SmartPtr(SmartPtr&&) = delete;
	//opterators
	SmartPtr& operator=(const SmartPtr& rhs)
	{
		if(this != &rhs)
		{
			point = rhs.point;
			count = ++rhs.count;
		}
		return *this;
	}
	SmartPtr& operator=(SmartPtr&&) = delete;
	T& operator*()
	{
		return *point;
	}
private:
	T* point;
	mutable unsigned int count;
};

void fun(SmartPtr<double> p)
{
	std::cout << *p << std::endl;
}

int main()
{
	SmartPtr<double> ptr1(new double(1.254));
	//SmartPtr<double> ptr2 = ptr1;
	fun(ptr1);
	std::cout << *ptr1 << std::endl;
	return 0;


}