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
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("Home:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textHome
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("0.00 km")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text2
			text: qsTr(" | ")
			horizontalAlignment: Text.AlignHCenter
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text3
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("Mission:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textMission
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("0.00 km")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text4
			horizontalAlignment: Text.AlignHCenter
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text5
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("Goal:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textGoal
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("0.00 km")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text6
			horizontalAlignment: Text.AlignHCenter
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text7
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("Lat:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textLat
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("0.00000")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text8
			horizontalAlignment: Text.AlignHCenter
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text9
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("Lon:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textLon
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("0.00000")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: text10
			horizontalAlignment: Text.AlignHCenter
			text: qsTr(" | ")
			color: "yellow"
			font.pixelSize: 12
			font.bold: true
		}

		Text {
			id: text11
			horizontalAlignment: Text.AlignHCenter
			text: qsTr("Alt:")
			color: "#ffffff"
			font.pixelSize: 12
		}

		Text {
			id: textAlt
			horizontalAlignment: Text.AlignHCenter
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

	function setMissionDistance(points)
	{
		if (points === null)
			textMission.text = "0.00 km"
		else
		{
			var distance = 0.0
			for (var i = 0; i < points.length - 1; i++)
				distance = distance + points[i].distanceTo(points[i + 1])
			textMission.text = (distance / 1000).toFixed(2) + " km"
		}
	}
}
