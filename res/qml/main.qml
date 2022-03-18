import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import QtLocation 5.6
import QtPositioning 5.6
import App.ProxyApp 1.0

ApplicationWindow {
	id: mainWindow
	x: 100
	y: 100
	width: 1200
	height: 800
	visible: true
	title: qsTr("Flight Map")

	property variant map
	property bool isWinOS: false
	property bool isMission: false

	property variant telemetryParams: null
	property variant hudParams: null
	property variant controlParams: null
	property variant missionParams: null

	Component.onCompleted: {
		logWindow.setText("Let's go!")
		isWinOS = proxy.isWindows()

		x = proxy.parameters["windowX"]
		y = proxy.parameters["windowY"]
		width = proxy.parameters["windowW"]
		height = proxy.parameters["windowH"]
		visibility = proxy.parameters["windowV"]

		if (proxy.parameters["telemetryWidget"] !== undefined)
			telemetryParams = proxy.parameters["telemetryWidget"]

		if (proxy.parameters["hudWidget"] !== undefined)
			hudParams = proxy.parameters["hudWidget"]

		if (proxy.parameters["controlWidget"] !== undefined)
			controlParams = proxy.parameters["controlWidget"]

		if (proxy.parameters["missionWidget"] !== undefined)
			missionParams = proxy.parameters["missionWidget"]
	}

	onClosing: {
		telemetryParams = { "visible": map.telemetryWidget.visible, "x":  map.telemetryWidget.x, "y": map.telemetryWidget.y }
		hudParams = { "visible": map.hudWidget.visible, "x":  map.hudWidget.x, "y": map.hudWidget.y }
		controlParams = { "visible": map.controlWidget.visible, "x":  map.controlWidget.x, "y": map.controlWidget.y }
		missionParams = { "visible": map.missionWidget.visible, "x":  map.missionWidget.x, "y": map.missionWidget.y }
		proxy.saveParameters({ "windowX": x, "windowY": y, "windowW": width, "windowH": height, "windowV": visibility,
							"mapZoom": map.zoomLevel, "mapLat": map.center.latitude, "mapLon": map.center.longitude,
							"mapTilt": map.tilt, "mapBear": map.bearing, "mapFov": map.fieldOfView, "telemetryWidget": telemetryParams,
							 "hudWidget": hudParams, "controlWidget": controlParams, "missionWidget": missionParams})
	}

	ConnectDlg {
		id: connectDlg
	}

	MapDlg {
		id: mapDlg
	}

	header: ToolBar {
		id: toolbar
		width: parent.width
		height: 36
		RowLayout {
			spacing: 0
			PanelButton {
				id: connectButton
				icon.source: "qrc:/img/connect.png"
				ToolTip.text: qsTr("Connect..")
				onClicked: connect()
			}
			PanelButton {
				id: disconnectButton
				icon.source: "qrc:/img/disconnect.png"
				ToolTip.text: qsTr("Disconnect")
				onClicked: disconnect()
			}
			PanelButton {
				id: mapButton
				icon.source: "qrc:/img/map.png"
				ToolTip.text: qsTr("Select map..")
				onClicked: selectMap()
			}
			PanelButton {
				id: homeButton
				icon.source: "qrc:/img/home.png"
				ToolTip.text: qsTr("Go home")
				onClicked: goHome()
			}
			ToolSeparator {}
			PanelButton {
				id: telemetryButton
				icon.source: "qrc:/img/telemetry.png"
				ToolTip.text: qsTr("Show telemetry widget")
				onClicked: showTelemetry()
			}
			PanelButton {
				id: hudButton
				icon.source: "qrc:/img/hud.png"
				ToolTip.text: qsTr("Show hud widget")
				onClicked: showHud()
			}
			PanelButton {
				id: controlButton
				icon.source: "qrc:/img/joystick.png"
				ToolTip.text: qsTr("Show control widget")
				onClicked: showControl()
			}
			PanelButton {
				id: missionButton
				icon.source: "qrc:/img/mission.png"
				ToolTip.text: qsTr("Show mission widget")
				onClicked: showMission()
			}
			ToolSeparator {}
			PanelButton {
				id: cameraButton
				icon.source: "qrc:/img/camera.png"
				ToolTip.text: qsTr("Turn on / off camera")
			}
			PanelButton {
				id: microphoneButton
				icon.source: "qrc:/img/microphone.png"
				ToolTip.text: qsTr("Turn on / off microphone")
			}
			PanelButton {
				id: speakerButton
				icon.source: "qrc:/img/speaker.png"
				ToolTip.text: qsTr("Turn on / off speaker")
			}
			ToolSeparator {}
			PanelButton {
				id: optionsButton
				icon.source: "qrc:/img/options.png"
				ToolTip.text: qsTr("Show options..")
			}
		}
	}

	footer: Rectangle {
		id: footer
		width: parent.width
		height: 23
		color: "#aaaaaa"

		RowLayout {
			anchors.fill: parent
			spacing: 5
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

			Text {
				id: footerMap
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				text: qsTr("Map:")
				color: "#000000"
				font.pixelSize: 12
			}

			Text {
				id: footerZoom
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.fillWidth: true
				text: qsTr("Zoom:")
				color: "#000000"
				font.pixelSize: 12
			}
		}
	}

	SplitView {
		id: splitView
		anchors.fill: parent
		orientation: Qt.Vertical

		Rectangle {
			id: mapRect
			border.width: 2
			border.color: "#aaaaaa"
			SplitView.fillWidth: true
			SplitView.fillHeight: true
			SplitView.minimumHeight: 100
			height: mainWindow.contentItem.height - logWindow.height

			MapSliders {
				id: sliders
				z: map.z + 3
				mapSource: map
				edge: Qt.LeftEdge
			}

			Component {
				id: mapComponent
				MapComponent { }
			}
		}

		LogWindow {
			id: logWindow
			SplitView.fillWidth: true
			SplitView.fillHeight: false
			SplitView.minimumHeight: 100
			height: 100
		}
	}

	function createMap(parameters)
	{
		var plugin  = Qt.createQmlObject ('import QtLocation 5.6; Plugin { name:"' + parameters["provider"] + '"}', mainWindow)

		if (parameters["provider"] === "mapboxgl") {

		}

		var zoomLevel = null
		var tilt = null
		var bearing = null
		var fov = null
		var center = null

		if (map) {
			zoomLevel = map.zoomLevel
			tilt = map.tilt
			bearing = map.bearing
			fov = map.fieldOfView
			center = map.center
			map.destroy()
		}
		else {
			zoomLevel = proxy.parameters["mapZoom"]
			tilt = proxy.parameters["mapTilt"]
			bearing = proxy.parameters["mapBear"]
			fov = proxy.parameters["mapFov"]
			center = QtPositioning.coordinate(proxy.parameters["mapLat"], proxy.parameters["mapLon"])
		}

		map = mapComponent.createObject(mapRect);
		map.plugin = plugin;
		for (var i = 0; i < map.supportedMapTypes.length; i++) {
			if (map.supportedMapTypes[i].name.localeCompare(parameters["mapType"]) === 0) {
				map.activeMapType = map.supportedMapTypes[i];
				break;
			}
		}

		if (zoomLevel != null) {
			map.zoomLevel = zoomLevel
			map.tilt = tilt
			map.bearing = bearing
			map.fieldOfView = fov
			map.center = center
		}

		map.forceActiveFocus()
		footerMap.text = "Map: " + parameters["provider"]  + ": " + parameters["mapType"]
		footerZoom.text = "Zoom: " + zoomLevel.toFixed(1)
		proxy.saveMap(parameters["provider"], parameters["mapType"])
	}

	function connect()
	{
		connectDlg.open()
	}

	function disconnect()
	{
		proxy.disconnect()
	}

	function showTelemetry()
	{
		map.showTelemetry()
	}

	function showHud()
	{
		map.showHud()
	}

	function showControl()
	{
		map.showControl()
	}

	function showMission()
	{
		map.showMission()
	}

	function selectMap()
	{
		mapDlg.open()
	}

	function goHome()
	{
		map.center = QtPositioning.coordinate(proxy.telemetry["lat"], proxy.telemetry["lon"])
		map.addQuadro(proxy.telemetry["lat"], proxy.telemetry["lon"])
	}
}
