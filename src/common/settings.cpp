#include "settings.h"
#include "common/log.h"
#include "common/common.h"

#include <json/json.h>

namespace common
{

Settings::Settings()
{
}

void Settings::LoadDefaults()
{
}

bool Settings::Load()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (!IsFileExists(kAppSettingsFile))
	{
		LoadDefaults();
		return true;
	}

	Json::Value root;
	std::ifstream ifs(kAppSettingsFile, std::ios::in);
	if (!ifs.is_open())
	{
		LOGE("[Settings] Can't create input stream: " << kAppSettingsFile);
		return false;
	}

	Json::CharReaderBuilder builder;
	JSONCPP_STRING error;
	if (!parseFromStream(builder, ifs, &root, &error))
	{
		LOGE("[Settings] Json parse error: " << error);
		return false;
	}

	// Settings objects
	Json::Value::Members rootMembers = root.getMemberNames();
	for (const string &group : rootMembers)
	{
		if (group == "widgets")
			continue;

		ParamMap map;
		Json::Value groupObject = root[group];

		Json::Value::Members members = groupObject.getMemberNames();
		for (const string &name : members)
		{
			if (groupObject[name].isString())
				map[name] = groupObject[name].asString();
			else if (groupObject[name].isBool())
				map[name] = groupObject[name].asBool();
			else if (groupObject[name].isInt())
				map[name] = groupObject[name].asInt();
			else if (groupObject[name].isUInt())
				map[name] = groupObject[name].asUInt();
			else if (groupObject[name].isDouble())
				map[name] = groupObject[name].asDouble();
			else if (groupObject[name].isInt64())
				map[name] = groupObject[name].asInt64();
		}

		_data[group] = map;
	}

	// Widgets
	Json::Value widgetsArray = root["widgets"];
	for (int i = 0; i < widgetsArray.size(); ++i)
	{
		Json::Value object = widgetsArray[i];
		string widgetName = object["name"].asString();

		common::AnyMap widgetMap;
		Json::Value::Members members = object.getMemberNames();
		for (const string &name : members)
		{
			if (object[name].isString())
				widgetMap.Set(name, object[name].asString());
			else if (object[name].isBool())
				widgetMap.Set(name, object[name].asBool());
			else if (object[name].isInt())
				widgetMap.Set(name, object[name].asInt());
			else if (object[name].isUInt())
				widgetMap.Set(name, object[name].asUInt());
			else if (object[name].isDouble())
				widgetMap.Set(name, object[name].asDouble());
			else if (object[name].isInt64())
				widgetMap.Set(name, object[name].asInt64());
		}

		_widgets[widgetName] = widgetMap;
	}

	return true;
}

void Settings::Save()
{
	std::unique_lock<std::mutex> lock(_mutex);
	Json::Value root(Json::objectValue);

	// Settings objects
	for (DataMapIterator it = _data.begin(); it != _data.end(); ++it)
	{
		const ParamMap &map = it->second;
		Json::Value groupObject(Json::objectValue);

		for (ParamMapConstIterator itParam = map.begin(); itParam != map.end(); ++itParam)
		{
			ParamVar var = itParam->second;
			std::visit([&](const auto& arg) {
				groupObject[itParam->first] = arg;
			}, var);
		}

		root[it->first] = groupObject;
	}

	// Widgets
	Json::Value widgetsArray(Json::arrayValue);
	for (auto &[key, val] : _widgets)
	{
		Json::Value object(Json::objectValue);
		object["name"] = key;

		for (auto &[widgetKey, widgetVal] : val)
		{
			if (widgetVal.type() == typeid(string))
				object[widgetKey] = std::any_cast<string>(widgetVal);
			else if (widgetVal.type() == typeid(bool))
				object[widgetKey] = std::any_cast<bool>(widgetVal);
			else if (widgetVal.type() == typeid(int))
				object[widgetKey] = std::any_cast<int>(widgetVal);
			else if (widgetVal.type() == typeid(unsigned int))
				object[widgetKey] = std::any_cast<unsigned int>(widgetVal);
			else if (widgetVal.type() == typeid(double))
				object[widgetKey] = std::any_cast<double>(widgetVal);
			else if (widgetVal.type() == typeid(time_t))
				object[widgetKey] = std::any_cast<time_t>(widgetVal);
		}

		widgetsArray.append(object);
	}
	root["widgets"] = widgetsArray;

	// Create file stream
	std::ofstream ofs(kAppSettingsFile, std::ios::out | std::ios::trunc);
	if (!ofs.is_open())
	{
		LOGE("[Settings] Can't create output stream: " << kAppSettingsFile);
		return;
	}

	// Write json text to file
	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	writer->write(root, &ofs);
}

}
