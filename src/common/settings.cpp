#include "settings.h"
#include "common/log.h"
#include "common/common.h"

#include <json/json.h>

namespace common
{

Settings::Settings()
{
}

void Settings::loadDefaults()
{
}

bool Settings::load()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (!isFileExists(kAppSettingsFile))
	{
		loadDefaults();
		return true;
	}

	Json::Value root;
	std::ifstream ifs(kAppSettingsFile, std::ios::in);
	if (!ifs.is_open())
	{
		LOGE("Can't create ifstream: " << kAppSettingsFile);
		return false;
	}

	Json::CharReaderBuilder builder;
	JSONCPP_STRING error;
	if (!parseFromStream(builder, ifs, &root, &error))
	{
		LOGE("Json parse error: " << error);
		return false;
	}

	// Common
	Json::Value commonObject = root["common"];
	Json::Value::Members members = commonObject.getMemberNames();
	for (const string &name : members)
	{
		if (commonObject[name].isString())
			_data[name] = commonObject[name].asString();
		else if (commonObject[name].isBool())
			_data[name] = commonObject[name].asBool();
		else if (commonObject[name].isInt())
			_data[name] = commonObject[name].asInt();
		else if (commonObject[name].isUInt())
			_data[name] = commonObject[name].asUInt();
		else if (commonObject[name].isDouble())
			_data[name] = commonObject[name].asDouble();
		else if (commonObject[name].isInt64())
			_data[name] = commonObject[name].asInt64();
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
				widgetMap.set(name, object[name].asString());
			else if (object[name].isBool())
				widgetMap.set(name, object[name].asBool());
			else if (object[name].isInt())
				widgetMap.set(name, object[name].asInt());
			else if (object[name].isUInt())
				widgetMap.set(name, object[name].asUInt());
			else if (object[name].isDouble())
				widgetMap.set(name, object[name].asDouble());
			else if (object[name].isInt64())
				widgetMap.set(name, object[name].asInt64());
		}

		_widgets[widgetName] = widgetMap;
	}

	return true;
}

void Settings::save()
{
	std::unique_lock<std::mutex> lock(_mutex);
	Json::Value root(Json::objectValue);

	// Common
	Json::Value commonObject(Json::objectValue);
	for (ParamMapIterator it = _data.begin(); it != _data.end(); ++it)
	{
		ParamVar var = it->second;
		std::visit([&](const auto& arg) {
			commonObject[it->first] = arg;
		}, var);
	}

	root["common"] = commonObject;

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
		LOGE("Can't create ofstream: " << kAppSettingsFile);
		return;
	}

	// Write json text to file
	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	writer->write(root, &ofs);
}

}
