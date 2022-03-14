import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15
import QtLocation 5.6
import QtPositioning 5.6

Rectangle {
	id: control
	visible: true
	width: {
		if (mainWindow.isWinOS)
			return 570
		else
			return 540
	}
	height: 28
	color: "#000000"
	border.width: 2
	border.color: "#ffffff"

	property real homeDistance: 0.0
	property real missionDistance: 0.0
	property real goalDistance: 0.0
	property real latitude: 0.0
	property real longitude: 0.0
	property real altitude: 0.0

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		drag.target: parent
	}

	RowLayout {
		anchors.fill: parent
		spacing: 0

		Text {
			id: text1
			Layout.leftMargin: 5
			text: qsTr("Home:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textHome
			text: qsTr("0.00 km")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text2
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text3
			text: qsTr("Mission:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textMission
			text: qsTr("0.00 km")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text4
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text5
			text: qsTr("Goal:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textGoal
			text: qsTr("0.00 km")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text6
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text7
			text: qsTr("Lat:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textLat
			text: qsTr("0.00000")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text8
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text9
			text: qsTr("Lon:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textLon
			text: qsTr("0.00000")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text10
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text11
			text: qsTr("Alt:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textAlt
			Layout.rightMargin: 5
			text: qsTr("0.0 m")
			color: "#ffffff"
			font.pixelSize: 12
		}
	}

	function setCoordinate(coord)
	{
		var distance = (coord.distanceTo(mapId.homePoint) / 1000).toFixed(2)
		textHome.text = distance + " km"
		textLat.text = coord.latitude.toFixed(5)
		textLon.text = coord.longitude.toFixed(5)
		textAlt.text = proxy.getAltitude(coord.latitude, coord.longitude).toFixed(1) + " m";
	}
}
