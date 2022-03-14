#include "proxy.h"
#include "common/log.h"
#include "common/common.h"
#include "main/dog.h"
#include "common/settings.h"
#include "common/devicetypes.h"
#include "data/telebox.h"

#include <QVariant>
#include <QDateTime>

namespace core
{

ProxyApp::ProxyApp(QObject *parent)
	: _active(false)
	, QObject(parent)
	, _qmlObject(nullptr)
{
}

ProxyApp::~ProxyApp()
{
	stopTelemetry();
}

void ProxyApp::init(QObject *object)
{
	_qmlObject = object;
	_protocols << "TCP" << "UDPC" << "UDPS";
	_devices << "Pixhawk" << "Raspi" << "Mavic";

	// Detect serial ports
	StringList ports = common::getComPorts();
	for (const string &port : ports)
		_protocols << port.c_str();

	// Default port
	string defaultPort = "TCP";
	if (!ports.empty())
		defaultPort = defaultPort[0];

	// Connect options
	_connection["protocol"] = common::Settings::instance()->get<string>("connectProtocol", defaultPort).c_str();
	_connection["device"] = common::Settings::instance()->get<string>("connectDevice", "Pixhawk").c_str();
	_connection["host"] = common::Settings::instance()->get<string>("connectHost", "192.168.10.1").c_str();
	_connection["port"] = common::Settings::instance()->get<int>("connectPort", 15000);
	_connection["baudrate"] = common::Settings::instance()->get<int>("connectBaudrate", 57600);

	// Telemetry
	_telemetry["alt"] = 0.0;
	_telemetry["roll"] = 0.0;
	_telemetry["pitch"] = 0.0;
	_telemetry["yaw"] = 0.0;
	_telemetry["grSpeed"] = 0.0;
	_telemetry["airSpeed"] = 0.0;
	_telemetry["gps"] = "NO_GPS";
	_telemetry["gpsHdop"] = 0.0;
	_telemetry["satellites"] = 0;
	_telemetry["heading"] = 0.0;
	_telemetry["batteryRem"] = 0.0;
	_telemetry["batteryVolt"] = 0.0;
	_telemetry["batteryAmp"] = 0.0;
	_telemetry["link"] = 0;
	_telemetry["mode"] = "Stabilize";
	_telemetry["status"] = "";

	// Window position
	_parameters["windowX"] = common::Settings::instance()->get<int>("windowX", 100);
	_parameters["windowY"] = common::Settings::instance()->get<int>("windowY", 100);
	_parameters["windowW"] = common::Settings::instance()->get<int>("windowW", 1200);
	_parameters["windowH"] = common::Settings::instance()->get<int>("windowH", 800);
	_parameters["windowV"] = common::Settings::instance()->get<int>("windowV", 2);

	// Map parameters
	_parameters["mapZoom"] = common::Settings::instance()->get<double>("mapZoom", 16.0);
	_parameters["mapLat"] = common::Settings::instance()->get<double>("mapLat", 59.938888);
	_parameters["mapLon"] = common::Settings::instance()->get<double>("mapLon", 30.315230);
	_parameters["mapTilt"] = common::Settings::instance()->get<double>("mapTilt", 0.0);
	_parameters["mapBear"] = common::Settings::instance()->get<double>("mapBear", 0.0);
	_parameters["mapFov"] = common::Settings::instance()->get<double>("mapFov", 0.0);

	// Widgets
	common::AnyMap telemetryMap = common::Settings::instance()->getWidget("telemetryWidget");
	if (!telemetryMap.empty())
	{
		QVariantMap telemetryWidget;
		anyMapToVariantMap(telemetryMap, telemetryWidget);
		_parameters["telemetryWidget"] = telemetryWidget;
	}

	common::AnyMap hudMap = common::Settings::instance()->getWidget("hudWidget");
	if (!hudMap.empty())
	{
		QVariantMap hudWidget;
		anyMapToVariantMap(hudMap, hudWidget);
		_parameters["hudWidget"] = hudWidget;
	}

	common::AnyMap controlMap = common::Settings::instance()->getWidget("controlWidget");
	if (!controlMap.empty())
	{
		QVariantMap controlWidget;
		anyMapToVariantMap(controlMap, controlWidget);
		_parameters["controlWidget"] = controlWidget;
	}

	common::AnyMap missionMap = common::Settings::instance()->getWidget("missionWidget");
	if (!missionMap.empty())
	{
		QVariantMap missionWidget;
		anyMapToVariantMap(missionMap, missionWidget);
		_parameters["missionWidget"] = missionWidget;
	}

	// Logger
	common::Log::create()->setCallback([this](const string &line) {
		emit logChanged(line.c_str());
	});

	// Telemetry
	_active = true;
}

void ProxyApp::saveParameters(const QVariantMap &params)
{
	// Window position
	common::Settings::instance()->set("windowX", params["windowX"].toInt());
	common::Settings::instance()->set("windowY", params["windowY"].toInt());
	common::Settings::instance()->set("windowW", params["windowW"].toInt());
	common::Settings::instance()->set("windowH", params["windowH"].toInt());
	common::Settings::instance()->set("windowV", params["windowV"].toInt());

	// Map parameters
	common::Settings::instance()->set("mapZoom", params["mapZoom"].toDouble());
	common::Settings::instance()->set("mapLat", params["mapLat"].toDouble());
	common::Settings::instance()->set("mapLon", params["mapLon"].toDouble());
	common::Settings::instance()->set("mapTilt", params["mapTilt"].toDouble());
	common::Settings::instance()->set("mapBear", params["mapBear"].toDouble());
	common::Settings::instance()->set("mapFov", params["mapFov"].toDouble());

	// Widgets
	common::AnyMap telemetryMap;
	variantMapToAnyMap(params["telemetryWidget"].toMap(), telemetryMap);
	common::Settings::instance()->setWidget("telemetryWidget", telemetryMap);

	common::AnyMap hudMap;
	variantMapToAnyMap(params["hudWidget"].toMap(), hudMap);
	common::Settings::instance()->setWidget("hudWidget", hudMap);

	common::AnyMap controlMap;
	variantMapToAnyMap(params["controlWidget"].toMap(), controlMap);
	common::Settings::instance()->setWidget("controlWidget", controlMap);

	common::AnyMap missionMap;
	variantMapToAnyMap(params["missionWidget"].toMap(), missionMap);
	common::Settings::instance()->setWidget("missionWidget", missionMap);

	common::Settings::instance()->save();
}

void ProxyApp::saveMap(const QString &provider, const QString &type)
{
	common::Settings::instance()->set("mapProvider", provider.toStdString());
	common::Settings::instance()->set("mapType", type.toStdString());
	common::Settings::instance()->save();
}

void ProxyApp::connect()
{
	if (Dog::instance()->isConnected())
	{
		LOGW("[ProxyApp] Device already connected!");
		return;
	}

	common::ConnectParams params = connectParams();

	std::stringstream host;
	host << _connection["protocol"].toString().toStdString() << " ";
	if (params.protocol.find(UART_NAME) == 0 || params.protocol.find(PIXHAWK_NAME) == 0)
		host << "baudrate: " << params.baudrate;
	else
		host << "address: " << params.host << ":" << params.port;

	// Save connect optionss
	common::Settings::instance()->set("connectProtocol", params.protocol);
	common::Settings::instance()->set("connectDevice", params.device);
	common::Settings::instance()->set("connectHost", params.host);
	common::Settings::instance()->set("connectPort", params.port);
	common::Settings::instance()->set("connectBaudrate", params.baudrate);
	common::Settings::instance()->save();

	LOG("[ProxyApp] Connecting to " << host.str());
	if (!Dog::instance()->connect(params))
	{
		LOGE("[ProxyApp] Can't connect to device! " << host.str());
		return;
	}

	LOG("[ProxyApp] Connected to " << params.device);
	startTelemetry();
}

void ProxyApp::disconnect()
{
	if (!Dog::instance()->isConnected())
	{
		LOGW("[ProxyApp] There is no connected devices!");
		return;
	}

	Dog::instance()->disconnect();
	stopTelemetry();
	LOG("[ProxyApp] Device disconnected!");
}

void ProxyApp::startTelemetry()
{
	_active = true;
	_telemetryThread = std::thread([this]() {
		data::TeleBoxPtr telebox = core::Dog::instance()->telebox();
		if (telebox == nullptr)
		{
			LOGC("[ProxyApp] There is no telemetry object!");
			return;
		}

		while (_active)
		{
			_telemetryMutex.lock();
			_telemetry["alt"] = telebox->alt;
			_telemetry["roll"] = telebox->roll;
			_telemetry["pitch"] = telebox->pitch;
			_telemetry["yaw"] = telebox->yaw;
			_telemetry["grSpeed"] = telebox->groundspeed;
			_telemetry["airSpeed"] = telebox->airspeed;
			_telemetry["gps"] = QString(telebox->gpstype.c_str());
			_telemetry["gpsHdop"] = telebox->eph;
			_telemetry["satellites"] = telebox->satellites_visible;
			_telemetry["heading"] = telebox->heading;
			_telemetry["batteryRem"] = telebox->battery_remaining;
			_telemetry["batteryVolt"] = telebox->voltage_battery;
			_telemetry["batteryAmp"] = telebox->current_battery;
			_telemetry["link"] = 100 - telebox->drop_rate_comm;
			_telemetry["mode"] = QString(telebox->flightmode.c_str());
			_telemetry["lat"] = telebox->lat;
			_telemetry["lon"] = telebox->lon;

			if (!telebox->statustext.empty())
			{
				LOG(telebox->statustext);
				telebox->statustext.clear();
			}

			_telemetryMutex.unlock();
			emit telemetryChanged();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});
}

void ProxyApp::stopTelemetry()
{
	_active = false;
	if (_telemetryThread.joinable())
		_telemetryThread.join();
}

bool ProxyApp::isWindows()
{
#ifdef WIN32
	return true;
#else
	return false;
#endif
}

float ProxyApp::getAltitude(float lat, float lon)
{
	return Dog::instance()->getAltitude(lat, lon);
}

common::ConnectParams ProxyApp::connectParams() const
{
	common::ConnectParams params;
	params.protocol = _connection["protocol"].toString().toStdString();
	params.device = _connection["device"].toString().toStdString();
	params.host = _connection["host"].toString().toStdString();
	params.port = _connection["port"].toUInt();
	params.baudrate = _connection["baudrate"].toUInt();
	return params;
}

void ProxyApp::variantMapToAnyMap(const QVariantMap &variantMap, common::AnyMap &anyMap) const
{
	for (QVariantMap::const_iterator it = variantMap.begin(); it != variantMap.end(); ++it)
	{
		QVariant::Type type = it.value().type();
		switch (type)
		{
		case QVariant::Type::String:
			anyMap.set(it.key().toStdString(), it.value().toString().toStdString());
			break;

		case QVariant::Type::Bool:
			anyMap.set(it.key().toStdString(), it.value().toBool());
			break;

		case QVariant::Type::Int:
			anyMap.set(it.key().toStdString(), it.value().toInt());
			break;

		case QVariant::Type::UInt:
			anyMap.set(it.key().toStdString(), it.value().toUInt());
			break;

		case QVariant::Type::Double:
			anyMap.set(it.key().toStdString(), it.value().toDouble());
			break;

		case QVariant::Type::Time:
			anyMap.set(it.key().toStdString(), it.value().toDateTime().toTime_t());
			break;
		}
	}
}

void ProxyApp::anyMapToVariantMap(common::AnyMap &anyMap, QVariantMap &variantMap) const
{
	for (const auto &[key, val] : anyMap)
	{
		QString variantKey(key.c_str());
		if (val.type() == typeid(string))
			variantMap[variantKey] = std::any_cast<string>(val).c_str();
		else if (val.type() == typeid(bool))
			variantMap[variantKey] = std::any_cast<bool>(val);
		else if (val.type() == typeid(int))
			variantMap[variantKey] = std::any_cast<int>(val);
		else if (val.type() == typeid(unsigned int))
			variantMap[variantKey] = std::any_cast<unsigned int>(val);
		else if (val.type() == typeid(double))
			variantMap[variantKey] = std::any_cast<double>(val);
		else if (val.type() == typeid(time_t))
			variantMap[variantKey] = QDateTime().fromTime_t(std::any_cast<time_t>(val));
	}
}

}
