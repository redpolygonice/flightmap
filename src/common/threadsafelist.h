#ifndef THREADSAFELIST_H
#define THREADSAFELIST_H

#include "common/types.h"
#include <list>

namespace common
{

template <class T>
class ThreadSafeList
{
private:
	std::list<T> _data;
	mutable std::mutex _mutex;

public:
	ThreadSafeList() {}
	~ThreadSafeList() {}

public:
	typedef typename std::list<T>::iterator iterator;
	iterator Begin() { return _data.begin(); }
	iterator End() { return _data.end(); }
	void Push(const T &element)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_data.push_back(element);
	}
	T Front()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return nullptr;

		T element = _data.front();
		return element;
	}
	T Pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return nullptr;

		T element = _data.front();
		_data.pop_front();
		return element;
	}
	void Remove(const T &element)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return;

		_data.remove(element);
	}
	bool Empty() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		bool result = _data.empty();
		return result;
	}
	size_t Size() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		size_t count = _data.size();
		return count;
	}
	void Clear()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return;

		_data.clear();
	}
};

}

#endif // THREADSAFELIST_H
