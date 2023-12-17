#include "proxy.h"
#include "common/anymap.h"
#include "common/log.h"
#include "common/common.h"
#include "main/broker.h"
#include "common/devicetypes.h"
#include "data/qtcoordinate.h"

#include <QVariant>
#include <QDateTime>
#include <QGeoCoordinate>
#include <QDataStream>
#include <QFileInfo>
#include <QFile>
#include <mutex>

namespace core
{

ProxyApp::ProxyApp(QObject *parent)
	: _active(false)
	, QObject(parent)
	, _qmlObject(nullptr)
	, _settings(common::GetSettings())
	, _telebox(nullptr)
{
}

ProxyApp::~ProxyApp()
{
	stopTelemetry();
}

void ProxyApp::processKeyDown(int key)
{
	Broker::instance()->ProcessKeyDown(static_cast<KeyCode>(key));
}

void ProxyApp::processKeyUp(int key)
{
	Broker::instance()->ProcessKeyUp(static_cast<KeyCode>(key));
}

void ProxyApp::arm(bool arm)
{
	Broker::instance()->Arm(arm);
}

void ProxyApp::loiter()
{
	Broker::instance()->Loiter();
}

void ProxyApp::autom()
{
	Broker::instance()->Auto();
}

void ProxyApp::rtl()
{
	Broker::instance()->rtl();
}

void ProxyApp::beep()
{
}

void ProxyApp::writeChannel(int index, int rc)
{
	Broker::instance()->WriteChannel(index, rc);
}

void ProxyApp::moveUp(bool stop)
{
	Broker::instance()->MoveUp(stop);
}

void ProxyApp::moveDown(bool stop)
{
	Broker::instance()->MoveDown(stop);
}

void ProxyApp::moveLeft(bool stop)
{
	Broker::instance()->MoveLeft(stop);
}

void ProxyApp::moveRight(bool stop)
{
	Broker::instance()->MoveRight(stop);
}

void ProxyApp::moveForward(bool stop)
{
	Broker::instance()->MoveForward(stop);
}

void ProxyApp::moveBack(bool stop)
{
	Broker::instance()->MoveBack(stop);
}

void ProxyApp::turnLeft(bool stop)
{
	Broker::instance()->TurnLeft(stop);
}

void ProxyApp::turnRight(bool stop)
{
	Broker::instance()->TurnRight(stop);
}

void ProxyApp::saveMissionToFile(const QString &fileName, const QVariantMap &params)
{
	common::AnyMap anyParams;
	VariantMapToAnyMap(params, anyParams);

	data::CoordinateList points;
	VariantMapListToCoordinateList(_missionPoints, points);

	Broker::instance()->SaveMissionToFile(fileName.toStdString(), points, anyParams);
	_settings->Set("missionFilePath", QFileInfo(fileName).path().toStdString());
}

QVariantMap ProxyApp::loadMissionFromFile(const QString &fileName)
{
	common::AnyMap anyParams;
	data::CoordinateList points;
	if (!Broker::instance()->LoadMissionFromFile(fileName.toStdString(), points, anyParams))
		return QVariantMap();

	QVariantMap params;
	AnyMapToVariantMap(anyParams, params);
	CoordinateListToVariantMapList(points, _missionPoints);

	_settings->Set("missionFilePath", QFileInfo(fileName).path().toStdString());
	return params;
}

QString ProxyApp::imageLocation()
{
	return QString::fromStdString(Broker::instance()->ImageLocation());
}

bool ProxyApp::imageExists()
{
	return QFile::exists(imageLocation());
}

void ProxyApp::startCamera()
{
	Broker::instance()->StartCamera();
}

void ProxyApp::stopCamera()
{
	Broker::instance()->StopCamera();
}

void ProxyApp::log(const QString &text, int level)
{
	common::log(text.toStdString(), static_cast<common::Log::Level>(level));
}

void ProxyApp::createMission(const QVariantList &points, const QVariantMap &params)
{
	if (points.isEmpty())
		return;

	VariantMapToAnyMap(params, _missionParams);

	data::CoordinateList coordinateList;
	VariantListToCoordinateList(points, coordinateList);

	//for (const data::CoordinatePtr &point : coordinateList)
		//LOGD("Lat: " << point->lat() << ", Lon: " << point->lon());

	data::CoordinateList missionPoints;
	if (!Broker::instance()->CreateMission(coordinateList, missionPoints, _missionParams))
		return;

	CoordinateListToVariantMapList(missionPoints, _missionPoints);
	emit missionPointsChanged();
}

void ProxyApp::updateMissionPoint(const QVariant &point, int number)
{
	if (number < 0 || number >= _missionPoints.size())
		return;

	QGeoCoordinate geoCoordinate = point.value<QGeoCoordinate>();
	_missionPoints[number]["lat"] = geoCoordinate.latitude();
	_missionPoints[number]["lon"] = geoCoordinate.longitude();
	_missionPoints[number]["alt"] = geoCoordinate.altitude();
}

void ProxyApp::clearMission()
{
	Broker::instance()->ClearMission();
	_missionPoints.clear();
	_missionParams.Clear();
}

void ProxyApp::writeMission()
{
	data::CoordinateList points;
	VariantMapListToCoordinateList(_missionPoints, points);
	Broker::instance()->WriteMissionAsync(points, _missionParams);
}

void ProxyApp::readMission()
{
	Broker::instance()->ReadMissionAsync([this](const data::CoordinateList &points) {
		CoordinateListToVariantMapList(points, _missionPoints);
		emit missionPointsChanged();
	});
}

void ProxyApp::Init(QObject *object)
{
	_qmlObject = object;
	_protocols << "TCP" << "UDPC" << "UDPS" << "Mock";
	_devices << "Pixhawk" << "Raspi";

	// Detect serial ports
	StringList ports = common::GetComPorts();
	for (const string &port : ports)
		_protocols << port.c_str();

	// Default port
	string defaultPort = "TCP";
	if (!ports.empty())
		defaultPort = defaultPort[0];

	// Connect options
	_connection["protocol"] = _settings->Get<string>("connectProtocol", defaultPort).c_str();
	_connection["device"] = _settings->Get<string>("connectDevice", "Pixhawk").c_str();
	_connection["host"] = _settings->Get<string>("connectHost", "192.168.10.1").c_str();
	_connection["port"] = _settings->Get<int>("connectPort", 15000);
	_connection["baudrate"] = _settings->Get<int>("connectBaudrate", 57600);

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
	_telemetry["number"] = 0;
	_telemetry["status"] = 0;

	// Window position
	_parameters["windowX"] = _settings->Get<int>("windowX", 100);
	_parameters["windowY"] = _settings->Get<int>("windowY", 100);
	_parameters["windowW"] = _settings->Get<int>("windowW", 1200);
	_parameters["windowH"] = _settings->Get<int>("windowH", 800);
	_parameters["windowV"] = _settings->Get<int>("windowV", 2);

	// Map parameters
	_parameters["mapZoom"] = _settings->Get<double>("mapZoom", 16.0);
	_parameters["mapLat"] = _settings->Get<double>("mapLat", 59.938888);
	_parameters["mapLon"] = _settings->Get<double>("mapLon", 30.315230);
	_parameters["mapTilt"] = _settings->Get<double>("mapTilt", 0.0);
	_parameters["mapBear"] = _settings->Get<double>("mapBear", 0.0);
	_parameters["mapFov"] = _settings->Get<double>("mapFov", 0.0);

	// Mission parameters
	_parameters["missionPolygon"] = _settings->Get<bool>("missionPolygon", false);
	_parameters["missionAlt"] = _settings->Get<int>("missionAlt", 50);
	_parameters["missionAltType"] = _settings->Get<int>("missionAltType", 0);
	_parameters["missionAltCheck"] = _settings->Get<bool>("missionAltCheck", true);
	_parameters["missionHorizStep"] = _settings->Get<int>("missionHorizStep", 50);
	_parameters["missionHorizStepCheck"] = _settings->Get<bool>("missionHorizStepCheck", false);
	_parameters["missionVertStep"] = _settings->Get<int>("missionVertStep", 50);
	_parameters["missionVertStepCheck"] = _settings->Get<bool>("missionVertStepCheck", false);
	_parameters["missionStep"] = _settings->Get<int>("missionStep", 50);
	_parameters["missionFilePath"] = _settings->Get<string>("missionFilePath").c_str();

	// Camera parameters
	_parameters["cameraWidth"] = _settings->Get<int>("cameraWidth", 640);
	_parameters["cameraHeight"] = _settings->Get<int>("cameraHeight", 480);
	_parameters["cameraQuality"] = _settings->Get<int>("cameraQuality", 50);
	_parameters["cameraBrightness"] = _settings->Get<int>("cameraBrightness", 50);

	// Widgets
	common::AnyMap telemetryMap = _settings->GetWidget("telemetryWidget");
	if (!telemetryMap.Empty())
	{
		QVariantMap telemetryWidget;
		AnyMapToVariantMap(telemetryMap, telemetryWidget);
		_parameters["telemetryWidget"] = telemetryWidget;
	}

	common::AnyMap hudMap = _settings->GetWidget("hudWidget");
	if (!hudMap.Empty())
	{
		QVariantMap hudWidget;
		AnyMapToVariantMap(hudMap, hudWidget);
		_parameters["hudWidget"] = hudWidget;
	}

	common::AnyMap controlMap = _settings->GetWidget("controlWidget");
	if (!controlMap.Empty())
	{
		QVariantMap controlWidget;
		AnyMapToVariantMap(controlMap, controlWidget);
		_parameters["controlWidget"] = controlWidget;
	}

	common::AnyMap missionMap = _settings->GetWidget("missionWidget");
	if (!missionMap.Empty())
	{
		QVariantMap missionWidget;
		AnyMapToVariantMap(missionMap, missionWidget);
		_parameters["missionWidget"] = missionWidget;
	}

	// Logger
	common::Log::Instance()->SetCallback([this](const string &line) {
		emit logChanged(line.c_str());
	});
}

void ProxyApp::saveParameters(const QVariantMap &params)
{
	// Window position
	_settings->Set("windowX", params["windowX"].toInt());
	_settings->Set("windowY", params["windowY"].toInt());
	_settings->Set("windowW", params["windowW"].toInt());
	_settings->Set("windowH", params["windowH"].toInt());
	_settings->Set("windowV", params["windowV"].toInt());

	// Map parameters
	_settings->Set("mapZoom", params["mapZoom"].toDouble());
	_settings->Set("mapLat", params["mapLat"].toDouble());
	_settings->Set("mapLon", params["mapLon"].toDouble());
	_settings->Set("mapTilt", params["mapTilt"].toDouble());
	_settings->Set("mapBear", params["mapBear"].toDouble());
	_settings->Set("mapFov", params["mapFov"].toDouble());

	// Mission parameters
	_settings->Set("missionPolygon", params["missionPolygon"].toBool());
	_settings->Set("missionAlt", params["missionAlt"].toInt());
	_settings->Set("missionAltType", params["missionAltType"].toInt());
	_settings->Set("missionAltCheck", params["missionAltCheck"].toBool());
	_settings->Set("missionHorizStep", params["missionHorizStep"].toInt());
	_settings->Set("missionHorizStepCheck", params["missionHorizStepCheck"].toBool());
	_settings->Set("missionVertStep", params["missionVertStep"].toInt());
	_settings->Set("missionVertStepCheck", params["missionVertStepCheck"].toBool());
	_settings->Set("missionStep", params["missionStep"].toInt());

	// Camera parameters
	_settings->Set("cameraWidth", params["cameraWidth"].toInt());
	_settings->Set("cameraHeight", params["cameraHeight"].toInt());
	_settings->Set("cameraQuality", params["cameraQuality"].toInt());
	_settings->Set("cameraBrightness", params["cameraBrightness"].toInt());

	// Widgets
	common::AnyMap telemetryMap;
	VariantMapToAnyMap(params["telemetryWidget"].toMap(), telemetryMap);
	_settings->SetWidget("telemetryWidget", telemetryMap);

	common::AnyMap hudMap;
	VariantMapToAnyMap(params["hudWidget"].toMap(), hudMap);
	_settings->SetWidget("hudWidget", hudMap);

	common::AnyMap controlMap;
	VariantMapToAnyMap(params["controlWidget"].toMap(), controlMap);
	_settings->SetWidget("controlWidget", controlMap);

	common::AnyMap missionMap;
	VariantMapToAnyMap(params["missionWidget"].toMap(), missionMap);
	_settings->SetWidget("missionWidget", missionMap);

	_settings->Save();
	_parameters = params;
}

void ProxyApp::saveMap(const QString &provider, const QString &type)
{
	_settings->Set("mapProvider", provider.toStdString());
	_settings->Set("mapType", type.toStdString());
	_settings->Save();
}

void ProxyApp::connect()
{
	common::ConnectParams params = connectParams();
	if (Broker::instance()->FindConnection(params))
	{
		LOGW("Device already connected!");
		return;
	}

	std::stringstream host;
	if (params.protocol == "Mock")
	{
		params.device = "Mock";
		host << "Mock device";
	}
	else
	{
		host << _connection["protocol"].toString().toStdString() << " ";
		if (params.protocol.find(UART_NAME) == 0 || params.protocol.find(PIXHAWK_NAME) == 0)
			host << "baudrate: " << params.baudrate;
		else
			host << "address: " << params.host << ":" << params.port;
	}

	// Save connect optionss
	_settings->Set("connectProtocol", params.protocol);
	_settings->Set("connectDevice", params.device);
	_settings->Set("connectHost", params.host);
	_settings->Set("connectPort", params.port);
	_settings->Set("connectBaudrate", params.baudrate);
	_settings->Save();

	LOG("Connecting to " << host.str() << " ..");

	string deviceName = host.str();
	Broker::instance()->ConnectAsync(params, [this, deviceName](bool result) {
		if (result)
		{
			LOG("Connected to " << deviceName << "!");

			_telebox = Broker::instance()->Telebox();
			if (_telebox == nullptr)
			{
				LOGC("There is no telemetry object!");
				return;
			}

			if (!_active)
				startTelemetry();
		}
		else
			LOGE("Can't connect to " << deviceName << "!") ;
	});
}

void ProxyApp::disconnect()
{
	Broker::instance()->Disconnect();
}

void ProxyApp::disconnect(int number)
{
	Broker::instance()->Disconnect(number);
}

void ProxyApp::startTelemetry()
{
	_active = true;
	_telemetryThread = std::thread([this]() {
		while (_active)
		{
			if (!_telebox->IsActive())
			{
				common::Sleep(100);
				continue;
			}

			_telemetryMutex.lock();
			_telemetry["alt"] = _telebox->alt;
			_telemetry["roll"] = _telebox->roll;
			_telemetry["pitch"] = _telebox->pitch;
			_telemetry["yaw"] = _telebox->yaw;
			_telemetry["grSpeed"] = _telebox->groundspeed;
			_telemetry["airSpeed"] = _telebox->airspeed;
			_telemetry["gps"] = QString::fromStdString(_telebox->gpstype);
			_telemetry["gpsHdop"] = _telebox->eph;
			_telemetry["satellites"] = _telebox->satellites_visible;
			_telemetry["heading"] = _telebox->heading;
			_telemetry["batteryRem"] = _telebox->battery_remaining;
			_telemetry["batteryVolt"] = _telebox->voltage_battery;
			_telemetry["batteryAmp"] = _telebox->current_battery;
			_telemetry["link"] = 100 - _telebox->drop_rate_comm;
			_telemetry["mode"] = QString::fromStdString(_telebox->flightmode);
			_telemetry["lat"] = _telebox->lat;
			_telemetry["lon"] = _telebox->lon;
			_telemetry["number"] = Broker::instance()->ActiveConnection()->Number();
			_telemetry["name"] = QString::fromStdString(Broker::instance()->ActiveDevice()->Name());
			_telemetry["status"] = 1;

			if (!_telebox->statustext.empty())
			{
				LOG(_telebox->statustext);
				_telebox->statustext.clear();
			}

			_telemetryMutex.unlock();
			emit telemetryChanged();
			common::Sleep(100);
		}
	});
}

void ProxyApp::stopTelemetry()
{
	_active = false;
	if (_telemetryThread.joinable())
		_telemetryThread.join();
}

void ProxyApp::setAcive(int number)
{
	std::lock_guard<std::mutex> lock(_teleboxMutex);
	Broker::instance()->SetAcive(number);
	_telebox = Broker::instance()->Telebox();
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
	return Broker::instance()->GetAltitude(lat, lon);
}

float ProxyApp::computeAlt(float lat, float lon, bool check, int type, int value)
{
	return Broker::instance()->ComputeAlt(lat, lon, check, type, value);
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

void ProxyApp::VariantMapToAnyMap(const QVariantMap &variantMap, common::AnyMap &anyMap) const
{
	for (QVariantMap::const_iterator it = variantMap.begin(); it != variantMap.end(); ++it)
	{
		QVariant::Type type = it.value().type();
		switch (type)
		{
		case QVariant::Type::String:
			anyMap.Set(it.key().toStdString(), it.value().toString().toStdString());
			break;

		case QVariant::Type::Bool:
			anyMap.Set(it.key().toStdString(), it.value().toBool());
			break;

		case QVariant::Type::Int:
			anyMap.Set(it.key().toStdString(), it.value().toInt());
			break;

		case QVariant::Type::UInt:
			anyMap.Set(it.key().toStdString(), it.value().toUInt());
			break;

		case QVariant::Type::Double:
			anyMap.Set(it.key().toStdString(), it.value().toDouble());
			break;

		case QVariant::Type::Time:
			anyMap.Set(it.key().toStdString(), it.value().toDateTime().toTime_t());
			break;
		}
	}
}

void ProxyApp::AnyMapToVariantMap(common::AnyMap &anyMap, QVariantMap &variantMap) const
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

void ProxyApp::VariantListToCoordinateList(const QVariantList &variantList, data::CoordinateList &coordinateList) const
{
	coordinateList.clear();

	for (QVariantList::const_iterator it = variantList.begin(); it != variantList.end(); ++it)
	{
		QGeoCoordinate geoCoordinate = (*it).value<QGeoCoordinate>();
		double lat = geoCoordinate.latitude();
		double lon = geoCoordinate.longitude();
		double alt = geoCoordinate.altitude();

		data::CoordinatePtr coordinate = data::QtCoordinate::create(lat, lon, alt);
		coordinateList.push_back(coordinate);
	}
}

void ProxyApp::CoordinateListToVariantList(const data::CoordinateList &coordinateList, QVariantList &variantList) const
{
	variantList.clear();

	for (data::CoordinateList::const_iterator it = coordinateList.begin(); it != coordinateList.end(); ++it)
	{
		data::CoordinatePtr coordinate = *it;
		QGeoCoordinate geoCoordinate(coordinate->Lat(), coordinate->Lon(), coordinate->Alt());

		QVariant varCoordinate;
		varCoordinate.setValue(geoCoordinate);
		variantList.push_back(varCoordinate);
	}
}

void ProxyApp::VariantMapListToCoordinateList(const VariantMapList &variantList, data::CoordinateList &coordinateList) const
{
	coordinateList.clear();

	for (VariantMapList::const_iterator it = variantList.begin(); it != variantList.end(); ++it)
	{
		QVariantMap varMap = *it;
		double lat = varMap["lat"].toDouble();
		double lon = varMap["lon"].toDouble();
		double alt = varMap["alt"].toDouble();
		bool base = varMap["base"].toBool();

		data::CoordinatePtr coordinate = data::QtCoordinate::create(lat, lon, alt);
		coordinate->SetBasePoint(base);
		coordinateList.push_back(coordinate);
	}
}

void ProxyApp::CoordinateListToVariantMapList(const data::CoordinateList &coordinateList, VariantMapList &variantList) const
{
	variantList.clear();

	for (data::CoordinateList::const_iterator it = coordinateList.begin(); it != coordinateList.end(); ++it)
	{
		data::CoordinatePtr coordinate = *it;
		QVariantMap varMap;
		varMap["lat"] = coordinate->Lat();
		varMap["lon"] = coordinate->Lon();
		varMap["alt"] = coordinate->Alt();
		varMap["base"] = coordinate->IsBasePoint();
		variantList.push_back(varMap);
	}
}

void ProxyApp::TestMissionPoints()
{
	// Connect
	common::ConnectParams params;
	params.device = "Mock";
	params.protocol = "Mock";

	LOG("Connecting to Mock device");
	if (!Broker::instance()->Connect(params))
	{
		LOGE("Can't connect to Mock device!");
		return;
	}

	LOG("Connected to Mock device");

	// Mission params
	common::AnyMap missionParams;
	missionParams.Set("polygon", false);
	missionParams.Set("homeLat", _parameters["mapLat"].toDouble());
	missionParams.Set("homeLon", _parameters["mapLon"].toDouble());
	missionParams.Set("alt", 50);
	missionParams.Set("altCheck", true);
	missionParams.Set("altType", 1);
	missionParams.Set("horizStep", 50);
	missionParams.Set("horizStepCheck", false);
	missionParams.Set("missionStep", 50);
	missionParams.Set("vertStep", 0);
	missionParams.Set("vertStepCheck", false);

	// Create mission
	data::CoordinateList coordinateList = {
		data::QtCoordinate::create(59.938173207038275, 30.312955486759535),
		data::QtCoordinate::create(59.93993597687384, 30.31454335448339),
		data::QtCoordinate::create(59.939978970085285, 30.31889926191994),
		data::QtCoordinate::create(59.93823770026752, 30.320916283101212)
	};

	data::CoordinateList missionPoints;
	if (!core::Broker::instance()->CreateMission(coordinateList, missionPoints, missionParams))
	{
		LOGE("Can't create mission!");
		return;
	}

	if (missionPoints.empty())
	{
		LOGE("Can't create mission!");
		return;
	}

	CoordinateListToVariantMapList(missionPoints, _missionPoints);
	emit missionPointsChanged();
}

void ProxyApp::TestMissionPolygon()
{
	// Connect
	common::ConnectParams params;
	params.device = "Mock";
	params.protocol = "Mock";

	LOG("Connecting to Mock device");
	if (!Broker::instance()->Connect(params))
	{
		LOGE("Can't connect to Mock device!");
		return;
	}

	LOG("Connected to Mock device");

	// Mission params
	common::AnyMap missionParams;
	missionParams.Set("polygon", true);
	missionParams.Set("homeLat", _parameters["mapLat"].toDouble());
	missionParams.Set("homeLon", _parameters["mapLon"].toDouble());
	missionParams.Set("alt", 50);
	missionParams.Set("altCheck", true);
	missionParams.Set("altType", 1);
	missionParams.Set("horizStep", 50);
	missionParams.Set("horizStepCheck", true);
	missionParams.Set("missionStep", 50);
	missionParams.Set("vertStep", 50);
	missionParams.Set("vertStepCheck", false);

	// Create mission
	data::CoordinateList coordinateList = {
		data::QtCoordinate::create(59.9392, 30.3113),
		data::QtCoordinate::create(59.9409, 30.3165),
		data::QtCoordinate::create(59.9396, 30.3227),
		data::QtCoordinate::create(59.9378, 30.3186),
		data::QtCoordinate::create(59.9375, 30.3144)
	};

	data::CoordinateList missionPoints;
	if (!core::Broker::instance()->CreateMission(coordinateList, missionPoints, missionParams))
	{
		LOGE("Can't create mission!");
		return;
	}

	if (missionPoints.empty())
	{
		LOGE("Can't create mission!");
		return;
	}

	CoordinateListToVariantMapList(missionPoints, _missionPoints);
	emit missionPointsChanged();
}

ProxyPtr Proxy()
{
	static ProxyPtr proxy = nullptr;
	if (proxy == nullptr)
		proxy.reset(new ProxyApp());
	return proxy;
}

}
