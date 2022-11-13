#ifndef PROXYAPP_H
#define PROXYAPP_H

#include "common/types.h"
#include "common/devicetypes.h"
#include "common/anymap.h"
#include "data/coordinate.h"

#include <QObject>
#include <QVariantMap>
#include <QStringList>

namespace core
{

// Proxy object for GUI
class ProxyApp : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList protocols READ protocols WRITE setProtocols)
	Q_PROPERTY(QStringList devices READ devices WRITE setDevices)
	Q_PROPERTY(QVariantMap connection READ connection WRITE setConnection)
	Q_PROPERTY(QVariantMap telemetry READ telemetry WRITE setTelemetry NOTIFY telemetryChanged)
	Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters)
	Q_PROPERTY(QVariantList missionPoints READ missionPoints WRITE setMissionPoints NOTIFY missionPointsChanged)

private:
	std::atomic_bool _active;
	QObject *_qmlObject;

	QStringList _protocols;
	QStringList _devices;
	QVariantMap _connection;
	QVariantMap _telemetry;
	QVariantMap _parameters;
	QVariantList _missionPoints;
	common::AnyMap _missionParams;

	std::thread _telemetryThread;
	std::mutex _telemetryMutex;

public:
	explicit ProxyApp(QObject *parent = nullptr);
	~ProxyApp();

signals:
	void logChanged(const QString &text);
	void telemetryChanged();
	void missionPointsChanged();

public slots:
	Q_INVOKABLE void log(const QString &text, int level = static_cast<int>(common::Log::Level::Info));
	Q_INVOKABLE void connect();
	Q_INVOKABLE void disconnect();
	Q_INVOKABLE void startTelemetry();
	Q_INVOKABLE void stopTelemetry();
	Q_INVOKABLE bool isWindows();
	Q_INVOKABLE float getAltitude(float lat, float lon);
	Q_INVOKABLE void saveMap(const QString &provider, const QString &type);
	Q_INVOKABLE void saveParameters(const QVariantMap &params);
	Q_INVOKABLE void createMission(const QVariantList &points, const QVariantMap &params);
	Q_INVOKABLE void clearMission();
	Q_INVOKABLE void writeMission();
	Q_INVOKABLE void readMission();
	Q_INVOKABLE void armDisarm(bool arm);
	Q_INVOKABLE void loiter();
	Q_INVOKABLE void autom();
	Q_INVOKABLE void rtl();

public:
	void testMission();
	void init(QObject *object = nullptr);
	common::ConnectParams connectParams() const;

	QStringList protocols() const { return _protocols; }
	void setProtocols(const QStringList &other) { _protocols = other; }

	QStringList devices() const { return _devices; }
	void setDevices(const QStringList &other) { _devices = other; }

	const QVariantMap &connection() const { return _connection; }
	void setConnection(const QVariantMap &map) { _connection = map; }

	const QVariantMap &telemetry() const { return _telemetry; }
	void setTelemetry(const QVariantMap &map) { _telemetry = map; }

	const QVariantMap &parameters() const { return _parameters; }
	void setParameters(const QVariantMap &map) { _parameters = map; }

	const QVariantList &missionPoints() const { return _missionPoints; }
	void setMissionPoints(const QVariantList &points) { _missionPoints = points; }

private:
	void variantMapToAnyMap(const QVariantMap &variantMap, common::AnyMap &anyMap) const;
	void anyMapToVariantMap(common::AnyMap &anyMap, QVariantMap &variantMap) const;
	void variantListToCoordinateList(const QVariantList &variantList, data::CoordinateList &coordinateList) const;
	void coordinateListToVariantList(const data::CoordinateList &coordinateList, QVariantList &variantList) const;
};

}

#endif // PROXYAPP_H
