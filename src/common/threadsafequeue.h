#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include "common/types.h"
#include <queue>

namespace common
{

template <class T>
class ThreadSafeQueue
{
private:
	std::queue<T> _data;
	mutable std::mutex _mutex;

public:
	ThreadSafeQueue() {}
	~ThreadSafeQueue() {}

public:
	void Push(const T &element)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_data.push(element);
	}
	void Push(T &&element)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_data.push(std::move(element));
	}
	T Pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return nullptr;

		T element = _data.front();
		_data.pop();
		return element;
	}
	bool Pop(T &element)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return false;

		element = _data.front();
		_data.pop();
		return true;
	}
	bool Pop(T &&element)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return false;

		element = std::move(_data.front());
		_data.pop();
		return true;
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
		std::queue<T> empty;
		std::swap(_data, empty);
	}
};

}

#endif // THREADSAFEQUEUE_H
