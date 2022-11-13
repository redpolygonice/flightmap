import QtQuick 2.0
import QtQuick.Controls 2.15
import QtLocation 5.6
import QtPositioning 5.6

Map {
	id: mapId
	anchors.fill: parent
	zoomLevel: 16

	property variant homePoint: null
	property bool isMissionCreated: false
	property bool isMission: false
	property bool isPolygon: false
	property variant quadro: null
	property alias telemetryWidget: telemetryWidgetId;
	property alias hudWidget: hudWidgetId;
	property alias controlWidget: controlWidgetId;
	property alias missionWidget: missionWidgetId;

	Component.onCompleted: {
		homePoint = QtPositioning.coordinate(proxy.parameters["mapLat"], proxy.parameters["mapLon"])
		homePin.coordinate = homePoint
		telemetryButton.checked = telemetryWidget.visible
		hudButton.checked = hudWidget.visible
		controlButton.checked = controlWidget.visible
		missionButton.checked = missionWidget.visible

		proxy.onMissionPointsChanged.connect(function() {
			drawMission()
		})
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
		border.width: 3
		border.color: 'red'
		opacity: 0.7
	}

	MapPolyline {
		id: redLine
		line.width: 3
		line.color: 'red'
		opacity: 0.7
	}

	MapPolyline {
		id: yellowLine
		line.width: 3
		line.color: 'yellow'
		opacity: 0.7
	}

	MapQuickItem {
		id: homePin
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
		cursorShape: {
			isMission ? Qt.CrossCursor : Qt.ArrowCursor
		}
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
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MapQuickItem {
			opacity: 0.7; sourceItem: Image { id: image; source: "qrc:/img/yellow-marker.png"; }}', mapId, 'dynamic')
		item.coordinate = mapId.toCoordinate(Qt.point(x, y))
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height
		mapId.addMapItem(item)

		if (isPolygon)
			polygon.addCoordinate(item.coordinate)
		else
			yellowLine.addCoordinate(item.coordinate)
	}

	function addItem(coordinate)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MapQuickItem {
			opacity: 0.7; sourceItem: Image { id: image; source: "qrc:/img/green-marker.png"; }}', mapId, 'dynamic')
		item.coordinate = coordinate
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height
		mapId.addMapItem(item)

		if (isPolygon)
			polygon.addCoordinate(item.coordinate)
		else
			redLine.addCoordinate(item.coordinate)
	}

	function addPoint(number, coordinate)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MissionPoint { }', mapId, 'dynamic')
		item.coordinate = coordinate
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height / 2
		item.number = number
		mapId.addMapItem(item)
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
		mapId.addMapItem(quadro)
	}

	function showHideWidget(widget, button)
	{
		if (widget.visible)
		{
			widget.visible = false
			button.checked = false
		}
		else
		{
			widget.visible = true
			button.checked = true
		}
	}

	function showTelemetry()
	{
		showHideWidget(telemetryWidget, telemetryButton)
	}

	function showControl()
	{
		showHideWidget(controlWidget, controlButton)
	}

	function showHud()
	{
		showHideWidget(hudWidget, hudButton)
	}

	function showMission()
	{
		showHideWidget(missionWidget, missionButton)
	}

	function clearAll()
	{
		var i = 0
		while (i < mapItems.length) {
			var item = mapItems[i]
			if (item !== homePin && item !== polygon && item !== redLine && item !== yellowLine)
				removeMapItem(item)
			else
				i = i + 1
		}

		while (redLine.path.length > 0) {
			redLine.removeCoordinate(redLine.path[0])
		}

		while (yellowLine.path.length > 0) {
			yellowLine.removeCoordinate(yellowLine.path[0])
		}

		while (polygon.path.length > 0) {
			polygon.removeCoordinate(polygon.path[0])
		}

		locationWidget.setMissionDistance(null)
	}

	function createMission(isPolygon)
	{
		var params = { "polygon": isPolygon,
			"homeLat": homePoint.latitude,
			"homeLon": homePoint.longitude,
			"alt": missionWidget.alt.value,
			"altType": missionWidget.altType.currentIndex,
			"altCheck": missionWidget.altCheck.checked,
			"missionStep": missionWidget.missionStep.value,
			"horizStep": missionWidget.horizStep.value,
			"horizStepCheck": missionWidget.horizStepCheck.checked,
			"vertStep": missionWidget.vertStep.value,
			"vertStepCheck": missionWidget.vertStepCheck.checked }

		if (isPolygon)
		{
			if (polygon.path.length <= 1)
			{
				proxy.log("Too small coordinate count!")
				return false
			}

			proxy.createMission(polygon.path, params)
		}
		else
		{
			if (yellowLine.path.length <= 1)
			{
				proxy.log("Too small coordinate count!")
				return false
			}

			proxy.createMission(yellowLine.path, params)
		}

		return true
	}

	function clearMission()
	{
		clearAll()
	}

	function drawMission()
	{
		clearAll()

		var points = proxy.missionPoints
		if (points.length === 0)
			return

		for (var i = 0; i < points.length; i++)
		{
			addPoint(i + 1, points[i])

			if (i !== 0 && i !== points.length - 1)
			{
				if (isPolygon)
					polygon.addCoordinate(points[i])
				else
					redLine.addCoordinate(points[i])
			}
		}

		yellowLine.addCoordinate(points[points.length - 2])
		yellowLine.addCoordinate(points[0])
		yellowLine.addCoordinate(points[1])

		locationWidget.setMissionDistance(points)
	}
}
