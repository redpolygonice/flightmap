#ifndef ANYMAP_H
#define ANYMAP_H

#include "common/types.h"
#include "log.h"

namespace common
{

// Container for store any type values
class AnyMap
{
private:
	mutable std::map<string, std::any> _data;

public:
	AnyMap();

public:
	typedef typename std::map<string, std::any>::const_iterator iterator;
	typedef typename std::map<string, std::any>::const_iterator const_iterator;
	iterator begin() { return _data.begin(); }
	iterator end() { return _data.end(); }
	const_iterator cbegin() { return _data.cbegin(); }
	const_iterator cend() { return _data.cend(); }

	void Push(const string &key, const std::any &value) { _data[key] = value; }

	void Set(const string &key, const std::any &value) { Push(key, value); }

	template<typename T> void Set(const string &key, const T &value)
	{
		Push(key, value);
	}

	template<typename T> T Get(const string &key, T defValue = T()) const
	{
		if (_data.find(key) == _data.end())
			return defValue;

		if (!_data[key].has_value())
			return defValue;

		T value = defValue;
		try
		{
			value = std::any_cast<T>(_data[key]);
		}
		catch (const std::exception &e)
		{
			LOGE("[AnyMap] get value error! " << e.what());
			return value;
		}

		return value;
	}

	void Clear() { _data.clear(); }

	bool Find(const string &key) const
	{
		if (_data.find(key) == _data.end())
			return false;
		return true;
	}

	bool Empty() const { return _data.empty(); }
};

}

#endif // ANYMAP_H
