#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#if QT_CONFIG(ssl)
#include <QSslSocket>
#endif

#include "proxy.h"
#include "broker.h"
#include "common/log.h"
#include "common/settings.h"
#include "messages/dispatcher.h"

#include <signal.h>

void sighandler(int /*sig*/)
{
	core::Broker::instance()->Stop();
	LOG("Program terminated");
	exit(1);
}

int main(int argc, char *argv[])
{

	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/img/map.png"));
	QQmlApplicationEngine engine;

#if QT_CONFIG(ssl)
	engine.rootContext()->setContextProperty("supportsSsl", QSslSocket::supportsSsl());
#else
	engine.rootContext()->setContextProperty("supportsSsl", false);
#endif

	common::Log::Create();
	common::Log::SetVerb(common::Log::Level::Debug);
	common::Settings::instance()->Load();

	core::Proxy()->Init();
	qmlRegisterType<core::ProxyApp>("App.ProxyApp", 1, 0, "ProxyApp");
	engine.rootContext()->setContextProperty("proxy", core::Proxy().get());

	const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [&](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);

	engine.load(url);
	QObject::connect(&engine, &QQmlApplicationEngine::quit, [] () {
		core::Broker::instance()->Stop();
		LOG("Quit application!");
		qApp->quit();
	});

	QObject *rootItem = engine.rootObjects().first();
	Q_ASSERT(rootItem);

	// Start with map provider and type from settings
	QVariantMap parameters;
	parameters["provider"] = common::Settings::instance()->Get<string, common::commonT>("mapProvider").c_str();
	parameters["mapType"] = common::Settings::instance()->Get<string, common::commonT>("mapType").c_str();

	if (parameters["provider"].toString().isEmpty() || parameters["mapType"].toString().isEmpty())
	{
		parameters["provider"] = "mapboxgl";
		parameters["mapType"] = "mapbox://styles/mapbox/satellite-v9";
	}

	QMetaObject::invokeMethod(rootItem, "createMap", Q_ARG(QVariant, QVariant::fromValue(parameters)));
	message::Dispatcher::instance()->Start();

	// Debug
	//proxy.testMissionPolygon();
	//proxy.testMissionPoints();

	return app.exec();
}
