#ifndef PROXYAPP_H
#define PROXYAPP_H

#include "common/types.h"
#include "common/devicetypes.h"
#include "common/anymap.h"

#include <QObject>
#include <QVariantMap>
#include <QStringList>

namespace core
{

// Proxy object for GUI
class ProxyApp : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString log READ log WRITE setLog NOTIFY logChanged)
	Q_PROPERTY(QStringList protocols READ protocols WRITE setProtocols)
	Q_PROPERTY(QStringList devices READ devices WRITE setDevices)
	Q_PROPERTY(QVariantMap connection READ connection WRITE setConnection)
	Q_PROPERTY(QVariantMap telemetry READ telemetry WRITE setTelemetry NOTIFY telemetryChanged)
	Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters)

private:
	std::atomic_bool _active;
	QObject *_qmlObject;

	QString _log;
	QStringList _protocols;
	QStringList _devices;
	QVariantMap _connection;
	QVariantMap _telemetry;
	QVariantMap _parameters;

	std::thread _telemetryThread;
	std::mutex _telemetryMutex;

public:
	explicit ProxyApp(QObject *parent = nullptr);
	~ProxyApp();

signals:
	void logChanged(const QString &text);
	void telemetryChanged();

public slots:
	Q_INVOKABLE void connect();
	Q_INVOKABLE void disconnect();
	Q_INVOKABLE void startTelemetry();
	Q_INVOKABLE void stopTelemetry();
	Q_INVOKABLE bool isWindows();
	Q_INVOKABLE float getAltitude(float lat, float lon);
	Q_INVOKABLE void saveMap(const QString &provider, const QString &type);
	Q_INVOKABLE void saveParameters(const QVariantMap &params);

public:
	void init(QObject *object = nullptr);
	common::ConnectParams connectParams() const;

	QString log() const { return _log; }
	void setLog(const QString &other) { _log = other; }

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

private:
	void variantMapToAnyMap(const QVariantMap &variantMap, common::AnyMap &anyMap) const;
	void anyMapToVariantMap(common::AnyMap &anyMap, QVariantMap &variantMap) const;
};

}

#endif // PROXYAPP_H
