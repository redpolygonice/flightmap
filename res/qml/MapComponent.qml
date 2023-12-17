import QtQuick 2.0
import QtQuick.Controls 2.15
import QtLocation 5.6
import QtPositioning 5.6

Map {
	id: mapId
	anchors.fill: parent
	zoomLevel: 16

	property variant homePoint: null
	property bool missionCreated: false
	property bool isMission: false
	property alias telemetryWidget: telemetryWidgetId;
	property alias hudWidget: hudWidgetId;
	property alias controlWidget: controlWidgetId;
	property alias missionWidget: missionWidgetId;
	property variant quadrolist: []

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

		proxy.onMapZoomChanged.connect(function(zoom) {
			zoomLevel = zoom
			footerZoom.text = "Zoom: " + mapId.zoomLevel.toFixed(1)
		})

		if (mainWindow.telemetryParams === null)
			showHideWidget(telemetryWidget, telemetryButton)

		if (mainWindow.hudParams === null)
			showHideWidget(hudWidget, hudButton)

		if (mainWindow.controlParams === null)
			showHideWidget(controlWidget, controlButton)

		if (mainWindow.missionParams === null)
			showHideWidget(missionWidget, missionButton)
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
		//x: mainWindow.telemetryParams === null ? 10 : mainWindow.telemetryParams["x"]
		//y: mainWindow.telemetryParams === null ? 10 : mainWindow.telemetryParams["y"]
		x: 10
		y: 10
		z: 2
	}

	HudWidget {
		id: hudWidgetId
		visible: mainWindow.hudParams === null ? false : mainWindow.hudParams["visible"]
		//x: mainWindow.hudParams === null ? 10 : mainWindow.hudParams["x"]
		//y:mainWindow.hudParams === null ? mapId.height - hudWidget.height - 10 : mainWindow.hudParams["y"]
		x: 10
		y: mapId.height - hudWidget.height - 10
		z: 3
	}

	ControlWidget {
		id: controlWidgetId
		visible: mainWindow.controlParams === null ? false : mainWindow.controlParams["visible"]
		//x: mainWindow.controlParams === null ? mapId.width / 2 - width / 2 : mainWindow.controlParams["x"]
		//y: mainWindow.controlParams === null ? mapId.height - controlWidget.height - 10 : mainWindow.controlParams["y"]
		x: mapId.width / 2 - width / 2
		y: mapId.height - controlWidget.height - 10
		z: 4
	}

	MissionWidget {
		id: missionWidgetId
		visible: mainWindow.missionParams === null ? false : mainWindow.missionParams["visible"]
		//x: mainWindow.missionParams === null ? mapId.width - width - 10 : mainWindow.missionParams["x"]
		//y: mainWindow.missionParams === null ? 10 : mainWindow.missionParams["y"]
		x: mapId.width - width - 10
		y: 10
		z: 5
	}

	MapPolygon {
		id: polygon
		border.width: 3
		border.color: 'yellow'
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
		propagateComposedEvents: true
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

	Keys.onPressed: {
		proxy.processKeyDown(event.key)
		event.accepted = true
	}

	Keys.onReleased: {
		if (!event.isAutoRepeat)
			proxy.processKeyUp(event.key)
		event.accepted = true
	}

	function getCoordinate(x, y)
	{
		return mapId.toCoordinate(Qt.point(x, y))
	}

	function setCoordinate(item, x, y)
	{
		for (var i = 0; i < mapItems.length; ++i)
		{
			if (item === mapItems[i])
			{
				var coord = mapItems[i].coordinate
				coord.altitude = proxy.computeAlt(coord.latitude, coord.longitude,
												  missionWidget.altCheck.checked, missionWidget.altType.currentIndex, missionWidget.alt.value)

				for (var j = 0; j < redLine.path.length; ++j)
				{
					if (redLine.path[j] === mapItems[i].fixedCoord)
					{
						redLine.replaceCoordinate(j, coord)
						break
					}
				}

				for (var k = 0; k < yellowLine.path.length; ++k)
				{
					if (yellowLine.path[k] === mapItems[i].fixedCoord)
					{
						yellowLine.replaceCoordinate(k, coord)
						break
					}
				}

				mapItems[i].fixedCoord = coord
				proxy.updateMissionPoint(coord, mapItems[i].number - 1)
				locationWidget.setMissionDistance(proxy.missionPoints)
				break
			}
		}
	}

	function addMarker(x, y)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MapQuickItem {
			opacity: 0.7; sourceItem: Image { id: image; source: "qrc:/img/yellow-marker.png"; }}', mapId, 'dynamic')
		item.coordinate = mapId.toCoordinate(Qt.point(x, y))
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height
		item.opacity = 0.7
		mapId.addMapItem(item)

		if (missionWidget.polygon)
			polygon.addCoordinate(item.coordinate)
		else
			yellowLine.addCoordinate(item.coordinate)
	}

	function addItem(coordinate)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MapQuickItem {
			opacity: 0.7; sourceItem: Image { id: image; source: "qrc:/img/yellow-marker.png"; }}', mapId, 'dynamic')
		item.coordinate.latitude = coordinate['lat']
		item.coordinate.longitude = coordinate['lon']
		item.coordinate.altitude = coordinate['alt']
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height
		item.opacity = 0.7
		mapId.addMapItem(item)

		if (missionWidget.polygon)
			polygon.addCoordinate(item.coordinate)
		else
			redLine.addCoordinate(item.coordinate)
	}

	function addMissionPoint(number, coordinate)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; MissionPoint { }', mapId, 'dynamic')
		item.coordinate.latitude = coordinate['lat']
		item.coordinate.longitude = coordinate['lon']
		item.coordinate.altitude = coordinate['alt']
		item.fixedCoord = item.coordinate
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height / 2
		item.opacity = 0.7
		item.number = number
		item.map = mapId
		mapId.addMapItem(item)
	}

	function addHomePoint(coordinate)
	{
		var item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; HomePoint { }', mapId, 'dynamic')
		item.coordinate.latitude = coordinate['lat']
		item.coordinate.longitude = coordinate['lon']
		item.coordinate.altitude = coordinate['alt']
		item.anchorPoint.x = item.sourceItem.width / 2
		item.anchorPoint.y = item.sourceItem.height / 2
		item.opacity = 0.7
		item.number = 1
		mapId.addMapItem(item)
	}

	function createQuadro(number)
	{
		homePin.visible = false

		var item = findQuadro(number)
		if (item === null)
		{

			item = Qt.createQmlObject('import QtQuick 2.12; import QtLocation 5.6; QuadroItem { }', mapId, 'dynamic')
			item.anchorPoint.x = item.sourceItem.width / 2
			item.anchorPoint.y = item.sourceItem.height / 2
			item.number = number
			item.active = active
			mapId.addMapItem(item)
			quadrolist.push(item)
			setActiveQuadro(number)
		}

		return item
	}

	function activeQuadro()
	{
		var item = quadrolist.find(function(element) {
			return element.active === true
		});

		if (item !== undefined)
			return item
		return null
	}

	function setActiveQuadro(number)
	{
		quadrolist.forEach(element => {
			if (element.number === number)
				element.active = true
			else
				element.active = false
			element.repaint()
		})
	}

	function findQuadro(number)
	{
		var item = quadrolist.find(function(element) {
			return element.number === number
		});

		if (item !== undefined)
			return item
		return null
	}

	function removeQuadro(number)
	{
		var index = quadrolist.findIndex(function(element) {
			return element.number === number
		});

		if (index >= 0)
		{
			mapId.removeMapItem(quadrolist[index])
			quadrolist[index].destroy()
			quadrolist.splice(index, 1)
		}

		if (quadrolist.length === 0)
			homePin.visible = true
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
			if (item !== homePin && item !== polygon && item !== redLine && item !== yellowLine && !quadrolist.includes(item))
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

	function createMission()
	{
		var quadro = activeQuadro()
		if (quadro !== null)
			homePoint = quadro.coordinate

		var params = missionWidget.getParams()

		if (missionWidget.polygon)
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

		proxy.log("Mission created!")
		return true
	}

	function clearMission()
	{
		clearAll()
	}

	function drawMission()
	{
		if (missionWidget.polygon === true)
			drawMissionPolygon()
		else
			drawMissionPoints()

		missionCreated = true
		locationWidget.setMissionDistance(proxy.missionPoints)
	}

	function drawMissionPoints()
	{
		clearAll()

		var points = proxy.missionPoints
		if (points.length === 0)
			return

		var count = 0
		for (var i = 0; i < points.length - 1; i++)
		{
			count++
			var point = points[i];
			if (i === 0)
				addHomePoint(point)
			else
				addMissionPoint(count, point)

			if (i !== 0 && i !== points.length - 1)
				redLine.addCoordinate(QtPositioning.coordinate(point['lat'], point['lon'], point['alt']))
		}

		yellowLine.addCoordinate(QtPositioning.coordinate(points[points.length - 2]['lat'], points[points.length - 2]['lon'], points[points.length - 2]['alt']))
		yellowLine.addCoordinate(QtPositioning.coordinate(points[0]['lat'], points[0]['lon'], points[0]['alt']))
		yellowLine.addCoordinate(QtPositioning.coordinate(points[1]['lat'], points[1]['lon'], points[1]['alt']))
		map.center = QtPositioning.coordinate(points[0]['lat'], points[0]['lon'], points[0]['alt'])
	}

	function drawMissionPolygon()
	{
		clearAll()

		var points = proxy.missionPoints
		if (points.length === 0)
			return

		// First draw base points
		var baseCount = 0
		for (var i = 0; i < points.length; i++)
		{
			var point = points[i];
			if (point['base'])
			{
				addItem(point)
				baseCount++
			}
		}

		// Draw mission grid points
		var count = 0
		for (var i = 0; i < points.length - baseCount; i++)
		{
			var point = points[i];
			if (point['base'])
				continue

			count++
			if (i === 0)
				addHomePoint(point)
			else if (i !== points.length - baseCount - 1)
				addMissionPoint(count, point)

			redLine.addCoordinate(QtPositioning.coordinate(point['lat'], point['lon'], point['alt']))
		}

		map.center = QtPositioning.coordinate(points[0]['lat'], points[0]['lon'], points[0]['alt'])
	}
}
