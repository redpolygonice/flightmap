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
	core::Broker::instance()->stop();
	LOG("Program terminated");
	exit(1);
}

int main(int argc, char *argv[])
{

	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/img/map.png"));
	QQmlApplicationEngine engine;

#if QT_CONFIG(ssl)
	engine.rootContext()->setContextProperty("supportsSsl", QSslSocket::supportsSsl());
#else
	engine.rootContext()->setContextProperty("supportsSsl", false);
#endif

	common::Log::create();
	common::Log::setVerb(common::Log::Level::Debug);
	common::Settings::instance()->load();

	core::ProxyApp proxy;
	proxy.init();
	qmlRegisterType<core::ProxyApp>("App.ProxyApp", 1, 0, "ProxyApp");
	engine.rootContext()->setContextProperty("proxy", &proxy);

	const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [&](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);

	engine.load(url);
	QObject::connect(&engine, &QQmlApplicationEngine::quit, [] () {
		core::Broker::instance()->stop();
		LOG("Quit application!");
		qApp->quit();
	});

	QObject *rootItem = engine.rootObjects().first();
	Q_ASSERT(rootItem);

	// Start with map provider and type from settings
	QVariantMap parameters;
	parameters["provider"] = common::Settings::instance()->get<string>("mapProvider").c_str();
	parameters["mapType"] = common::Settings::instance()->get<string>("mapType").c_str();

	if (parameters["provider"].toString().isEmpty() || parameters["mapType"].toString().isEmpty())
	{
		parameters["provider"] = "mapboxgl";
		parameters["mapType"] = "mapbox://styles/mapbox/satellite-v9";
	}

	QMetaObject::invokeMethod(rootItem, "createMap", Q_ARG(QVariant, QVariant::fromValue(parameters)));
	message::Dispatcher::instance()->start();

	// Debug
	//proxy.testMission();

	return app.exec();
}
