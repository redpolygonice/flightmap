#ifndef PROXYAPP_H
#define PROXYAPP_H

#include "common/types.h"
#include "common/settings.h"
#include "common/devicetypes.h"
#include "common/anymap.h"
#include "data/icoordinate.h"
#include "data/telebox.h"

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include <QList>

namespace core
{

typedef QList<QVariantMap> VariantMapList;

// Proxy object for GUI
class ProxyApp : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList protocols READ protocols WRITE setProtocols)
	Q_PROPERTY(QStringList devices READ devices WRITE setDevices)
	Q_PROPERTY(QVariantMap connection READ connection WRITE setConnection)
	Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
	Q_PROPERTY(QVariantMap telemetry READ telemetry WRITE setTelemetry NOTIFY telemetryChanged)
	Q_PROPERTY(QList<QVariantMap> missionPoints READ missionPoints WRITE setMissionPoints NOTIFY missionPointsChanged)

private:
	std::atomic_bool _active;
	QObject *_qmlObject;
	common::Settings *_settings;
	data::TeleBoxPtr _telebox;

	QStringList _protocols;
	QStringList _devices;
	QVariantMap _connection;
	QVariantMap _telemetry;
	QVariantMap _parameters;
	QList<QVariantMap> _missionPoints;
	common::AnyMap _missionParams;

	std::thread _telemetryThread;
	std::mutex _telemetryMutex;
	std::mutex _teleboxMutex;

public:
	explicit ProxyApp(QObject *parent = nullptr);
	~ProxyApp();

signals:
	void logChanged(const QString &text);
	void parametersChanged();
	void telemetryChanged();
	void missionPointsChanged();
	void disconnected(int number);
	void mapZoomChanged(double zoom);

public slots:
	Q_INVOKABLE void processKeyDown(int key);
	Q_INVOKABLE void processKeyUp(int key);
	Q_INVOKABLE void log(const QString &text, int level = static_cast<int>(common::Log::Level::Info));
	Q_INVOKABLE void connect();
	Q_INVOKABLE void disconnect();
	Q_INVOKABLE void disconnect(int number);
	Q_INVOKABLE void startTelemetry();
	Q_INVOKABLE void stopTelemetry();
	Q_INVOKABLE void setAcive(int number);
	Q_INVOKABLE bool isWindows();
	Q_INVOKABLE float getAltitude(float lat, float lon);
	Q_INVOKABLE float computeAlt(float lat, float lon, bool check, int type, int value);
	Q_INVOKABLE void saveMap(const QString &provider, const QString &type);
	Q_INVOKABLE void saveParameters(const QVariantMap &params);
	Q_INVOKABLE void createMission(const QVariantList &points, const QVariantMap &params);
	Q_INVOKABLE void updateMissionPoint(const QVariant &point, int number);
	Q_INVOKABLE void clearMission();
	Q_INVOKABLE void writeMission();
	Q_INVOKABLE void readMission();
	Q_INVOKABLE void arm(bool arm);
	Q_INVOKABLE void loiter();
	Q_INVOKABLE void autom();
	Q_INVOKABLE void rtl();
	Q_INVOKABLE void beep();
	Q_INVOKABLE void writeChannel(int index, int rc);
	Q_INVOKABLE void moveUp(bool stop = false);
	Q_INVOKABLE void moveDown(bool stop = false);
	Q_INVOKABLE void moveLeft(bool stop = false);
	Q_INVOKABLE void moveRight(bool stop = false);
	Q_INVOKABLE void moveForward(bool stop = false);
	Q_INVOKABLE void moveBack(bool stop = false);
	Q_INVOKABLE void turnLeft(bool stop = false);
	Q_INVOKABLE void turnRight(bool stop = false);
	Q_INVOKABLE void saveMissionToFile(const QString &fileName, const QVariantMap &params);
	Q_INVOKABLE QVariantMap loadMissionFromFile(const QString &fileName);

public:
	void TestMissionPoints();
	void TestMissionPolygon();

	void Init(QObject *object = nullptr);
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

	const QList<QVariantMap> &missionPoints() const { return _missionPoints; }
	void setMissionPoints(const QList<QVariantMap> &points) { _missionPoints = points; }

	void SetMapZoom(double zoom) { emit mapZoomChanged(zoom); }

private:
	void VariantMapToAnyMap(const QVariantMap &variantMap, common::AnyMap &anyMap) const;
	void AnyMapToVariantMap(common::AnyMap &anyMap, QVariantMap &variantMap) const;
	void VariantListToCoordinateList(const QVariantList &variantList, data::CoordinateList &coordinateList) const;
	void CoordinateListToVariantList(const data::CoordinateList &coordinateList, QVariantList &variantList) const;
	void VariantMapListToCoordinateList(const VariantMapList &variantList, data::CoordinateList &coordinateList) const;
	void CoordinateListToVariantMapList(const data::CoordinateList &coordinateList, VariantMapList &variantList) const;
};

using ProxyPtr = std::shared_ptr<ProxyApp>;

ProxyPtr Proxy();

}

#endif // PROXYAPP_H
