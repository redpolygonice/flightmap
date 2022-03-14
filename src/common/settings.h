#ifndef SETTINGS_H
#define SETTINGS_H

#include "common/types.h"
#include "common/singleton.h"
#include "common/anymap.h"

#include <type_traits>

namespace common
{

static const char kAppSettingsFile[] = "settings.json";

// Application settings
class Settings : public Singleton<Settings>
{
	friend class Singleton<Settings>;

private:
	ParamMap _data;
	std::map<string, common::AnyMap> _widgets;
	std::mutex _mutex;

private:
	Settings();
	void loadDefaults();

public:
	bool load();
	void save();

	template<typename T> T operator[] (const string &key)
	{
		return get<T>(key);
	}

	void set(const string &key, const ParamVar &value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_data[key] = value;
	}

	ParamVar get(const string &key)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _data[key];
	}

	template<typename T> T get(const string &key, T defValue = T())
	{
		std::unique_lock<std::mutex> lock(_mutex);
		T value = defValue;
		try
		{
			value = std::get<T>(_data[key]);
		}
		catch (const std::bad_variant_access& ex)
		{
			// Jsoncpp can convert double to int, check it
			if constexpr (std::is_same_v<T, double>)
			{
				double dblValue = value;
				if (dblValue == 0)
				{
					lock.unlock();
					return (double)get<int>(key);
				}
			}

			return defValue;
		}

		if constexpr (std::is_same_v<T, string>)
		{
			if (value.empty())
				return defValue;
		}
		else if (value == 0)
			return defValue;

		return value;
	}

	void setWidget(const string &name, const common::AnyMap &data)
	{
		_widgets[name] = data;
	}

	common::AnyMap getWidget(const string &name) const
	{
		std::map<string, common::AnyMap>::const_iterator it = _widgets.find(name);
		if (it != _widgets.end())
			return it->second;
		return common::AnyMap();
	}
};

}

#endif // SETTINGS_H
