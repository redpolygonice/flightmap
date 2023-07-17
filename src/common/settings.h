#ifndef SETTINGS_H
#define SETTINGS_H

#include "common/types.h"
#include "common/singleton.h"
#include "common/anymap.h"

#include <type_traits>

template <typename CharType = char, CharType... Chars>
class String {
  public:
	constexpr String() : value{Chars...} {}
  private:
	CharType value[sizeof... (Chars)];
};

template <typename CharType, CharType... Chars>
inline constexpr String<CharType, Chars...> operator""_conststr() {
  return String<CharType, Chars...>();
}

namespace common
{

constexpr char kAppSettingsFile[] = "settings.json";
constexpr char commonT[] = "common";
constexpr char missionT[] = "mission";

typedef std::map<string, ParamMap> DataMap;
typedef std::map<string, ParamMap>::iterator DataMapIterator;
typedef std::map<string, ParamMap>::const_iterator DataMapConstIterator;

typedef std::map<string, common::AnyMap> WidgetMap;
typedef std::map<string, common::AnyMap>::iterator WidgetMapIterator;
typedef std::map<string, common::AnyMap>::const_iterator WidgetMapConstIterator;

// Application settings
class Settings : public Singleton<Settings>
{
	friend class Singleton<Settings>;

private:
	DataMap _data;
	WidgetMap _widgets;
	std::mutex _mutex;

private:
	Settings();
	void LoadDefaults();

	Settings(const Settings &other) = delete;
	Settings(const Settings &&other) = delete;
	void operator= (const Settings &other) = delete;

public:
	bool Load();
	void Save();

	template<typename T> T operator[] (const string &key)
	{
		return Get<T>(key);
	}

	template<const char name[] = commonT> void Set(const string &key, const ParamVar &value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_data[string(name)][key] = value;
	}

	template<typename T, const char name[] = commonT> T Get(const string &key, T defValue = T())
	{
		std::unique_lock<std::mutex> lock(_mutex);
		T value = defValue;
		try
		{
			DataMapIterator it = _data.find(string(name));
			if (it == _data.end())
				return defValue;

			value = std::get<T>(it->second[key]);
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
					return (double)Get<int, name>(key);
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

	void SetWidget(const string &name, const AnyMap &data)
	{
		_widgets[name] = data;
	}

	AnyMap GetWidget(const string &name) const
	{
		std::map<string, AnyMap>::const_iterator it = _widgets.find(name);
		if (it != _widgets.end())
			return it->second;
		return AnyMap();
	}
};

}

#endif // SETTINGS_H
