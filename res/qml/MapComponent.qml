import QtQuick 2.0
import QtQuick.Controls 2.15
import QtLocation 5.6
import QtPositioning 5.6

Map {
	id: mapId
	anchors.fill: parent
	zoomLevel: 16

	property variant homePoint: null
	property bool isMission: false
	property variant quadro: null
	property alias telemetryWidget: telemetryWidgetId;
	property alias hudWidget: hudWidgetId;
	property alias controlWidget: controlWidgetId;
	property alias missionWidget: missionWidgetId;

	Component.onCompleted: {
		homePoint = QtPositioning.coordinate(proxy.parameters["mapLat"], proxy.parameters["mapLon"])
		greenPin.coordinate = homePoint
		telemetryButton.checked = telemetryWidget.visible
		hudButton.checked = hudWidget.visible
		controlButton.checked = controlWidget.visible
		missionButton.checked = missionWidget.visible
	}

	LocationWidget {
		id: locationWidget
		visible: true
		x: mapId.width / 2 - width / 2
		y: 0
		z: 1
	}

	TelemetryWidget {
		id: telemetryWidgetId
		visible: mainWindow.telemetryParams === null ? false : mainWindow.telemetryParams["visible"]
		x: mainWindow.telemetryParams === null ? 10 : mainWindow.telemetryParams["x"]
		y: mainWindow.telemetryParams === null ? 10 : mainWindow.telemetryParams["y"]
		z: 2
	}

	HudWidget {
		id: hudWidgetId
		visible: mainWindow.hudParams === null ? false : mainWindow.hudParams["visible"]
		x: mainWindow.hudParams === null ? 10 : mainWindow.hudParams["x"]
		y:mainWindow.hudParams === null ? mapId.height - hudWidget.height - 10 : mainWindow.hudParams["y"]
		z: 3
	}

	ControlWidget {
		id: controlWidgetId
		visible: mainWindow.controlParams === null ? false : mainWindow.controlParams["visible"]
		x: mainWindow.controlParams === null ? mapId.width / 2 - width / 2 : mainWindow.controlParams["x"]
		y: mainWindow.controlParams === null ? mapId.height - controlWidget.height - 10 : mainWindow.controlParams["y"]
		z: 4
	}

	MissionWidget {
		id: missionWidgetId
		visible: mainWindow.missionParams === null ? false : mainWindow.missionParams["visible"]
		x: mainWindow.missionParams === null ? mapId.width - width - 10 : mainWindow.missionParams["x"]
		y: mainWindow.missionParams === null ? 10 : mainWindow.missionParams["y"]
		z: 5
	}

	MapPolygon {
		id: polygon
		border.width: 2
		border.color: "red"
	}

	MapQuickItem {
		id: greenPin
		sourceItem: Image {
			id: imageGreenPin
			source: "qrc:/img/green-pin.png"
		}
		anchorPoint.x: imageGreenPin.width / 2
		anchorPoint.y: imageGreenPin.height
	}

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		onPressed: {
			if (mapId.isMission)
				addMarker(mouse.x, mouse.y)
		}
		onPositionChanged: {
			var coord = mapId.toCoordinate(Qt.point(mouse.x, mouse.y))
			locationWidget.setCoordinate(coord)
		}
		onWheel: {
			footerZoom.text = "Zoom: " + mapId.zoomLevel.toFixed(1)
			wheel.accepted = false
		}
	}

	function addMarker(x, y)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MapQuickItem { sourceItem: Image { id: image; source: "qrc:/img/green-marker.png"; }}', mapId, 'dynamic')
		item.coordinate = mapId.toCoordinate(Qt.point(x, y))
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height
		mapId.addMapItem(item);
		polygon.addCoordinate(item.coordinate)
	}

	function addQuadro(lat, lon)
	{
		if (quadro)
		{
			mapId.removeMapItem(quadro)
			quadro.destroy()
		}

		quadro = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MapQuickItem { sourceItem: Image { id: image; source: "qrc:/img/quadro.png"; }}', mapId, 'dynamic')
		quadro.coordinate = QtPositioning.coordinate(lat, lon)
		quadro.anchorPoint.x = quadro.sourceItem.width / 2
		quadro.anchorPoint.y = quadro.sourceItem.height
		mapId.addMapItem(quadro);
	}

	function showTelemetry()
	{
		if (telemetryWidget.visible)
		{
			telemetryWidget.visible = false
			telemetryButton.checked = false
		}
		else
		{
			telemetryWidget.visible = true
			telemetryButton.checked = true
		}
	}

	function showControl()
	{
		if (controlWidget.visible)
		{
			controlWidget.visible = false
			controlButton.checked = false
		}
		else
		{
			controlWidget.visible = true
			controlButton.checked = true
		}
	}

	function showHud()
	{
		if (hudWidget.visible)
		{
			hudWidget.visible = false
			hudButton.checked = false
		}
		else
		{
			hudWidget.visible = true
			hudButton.checked = true
		}
	}

	function showMission()
	{
		if (missionWidget.visible)
		{
			missionWidget.visible = false
			missionButton.checked = false
		}
		else
		{
			missionWidget.visible = true
			missionButton.checked = true
		}
	}
}
