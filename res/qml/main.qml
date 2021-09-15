import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import QtLocation 5.6
import QtPositioning 5.6

ApplicationWindow {
	width: 1200
	height: 800
	x: screen.width / 2 - width / 2
	y: screen.height / 2 - height / 2
	visible: true
	title: qsTr("Flight Map")

	header: ToolBar {
		width: parent.width
		height: 36
		RowLayout {
			spacing: 1
			ToolButton {
				Layout.preferredHeight: 30
				Layout.preferredWidth: 30
				icon.source: "qrc:/img/connect.png"
				ToolTip.visible: hovered
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("Connect ..")
				onClicked: connect()
			}
			ToolButton {
				Layout.preferredHeight: 30
				Layout.preferredWidth: 30
				icon.source: "qrc:/img/disconnect.png"
				ToolTip.visible: hovered
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("Disconnect")
				onClicked: disconnect()
			}
			ToolSeparator {}
		}
	}

	Plugin {
		id: mapPlugin
		name: "osm"
	}

	Map {
		id: mapId
		anchors.fill: parent
		plugin: mapPlugin
		center: homePoint
		zoomLevel: 16

		property variant homePoint: QtPositioning.coordinate(59.938888, 30.315230) // Spb

		Component.onCompleted: {
			greenPin.coordinate = homePoint
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
			onPressed: {
				addMarker(mouse.x, mouse.y)
			}
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

	function connect()
	{
	}

	function disconnect()
	{
	}
}
