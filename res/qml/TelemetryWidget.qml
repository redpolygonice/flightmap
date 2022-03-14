import QtQuick 2.0

Rectangle {
	id: control
	visible: true
	width: 150
	height: {
		if (mainWindow.isWinOS)
			return 300
		else
			return 312
	}
	color: "#000000"
	border.width: 2
	border.color: "#ffffff"

	property int pixelSize: 12

	Component.onCompleted: {
		update()
		proxy.onTelemetryChanged.connect(function() {
			update()
		})
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		drag.target: parent
	}

	Grid {
		id: grid
		anchors.fill: parent
		rows: 15
		columns: 2
		spacing: 5
		anchors.margins: 10

		Text {
			id: text1
			text: qsTr("Alt")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textAlt
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text3
			text: qsTr("Roll")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textRoll
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text5
			text: qsTr("Pitch")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textPitch
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text7
			text: qsTr("Yaw")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textYaw
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text9
			text: qsTr("GrSpeed")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textGrSpeed
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text11
			text: qsTr("AirSpeed")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textAirSpeed
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text13
			text: qsTr("Gps")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textGps
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text15
			text: qsTr("GpsHdop")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textGpsHdop
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text17
			text: qsTr("Satellites")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textSatellites
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text19
			text: qsTr("Heading")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textHeading
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text21
			text: qsTr("BatteryRem")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textBatteryRem
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text23
			text: qsTr("BatteryVolt")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textBatteryVolt
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text25
			text: qsTr("BatteryAmp")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textBatteryAmp
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text27
			text: qsTr("Link")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textLink
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: text29
			text: qsTr("Mode")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}

		Text {
			id: textMode
			text: qsTr("Text")
			color: "#ffffff"
			font.pixelSize: pixelSize
		}
	}

	function update() {
		textAlt.text = proxy.telemetry["alt"].toFixed(1) + "m"
		textRoll.text = proxy.telemetry["roll"].toFixed(1) + "^"
		textPitch.text = proxy.telemetry["pitch"].toFixed(1) + "^"
		textYaw.text = proxy.telemetry["yaw"].toFixed(1) + "^"
		textGrSpeed.text = proxy.telemetry["grSpeed"].toFixed(1) + "ms"
		textAirSpeed.text = proxy.telemetry["airSpeed"].toFixed(1) + "ms"
		textGps.text = proxy.telemetry["gps"]
		textGpsHdop.text = "v" + proxy.telemetry["gpsHdop"].toFixed(1)
		textSatellites.text = proxy.telemetry["satellites"]
		textHeading.text = proxy.telemetry["heading"] + "^"
		textBatteryRem.text = proxy.telemetry["batteryRem"] + "%"
		textBatteryVolt.text = proxy.telemetry["batteryVolt"] + "mV"
		textBatteryAmp.text = proxy.telemetry["batteryAmp"] + "mA"
		textLink.text = proxy.telemetry["link"] + "%"
		textMode.text = proxy.telemetry["mode"]
		hudWidget.update(proxy.telemetry["roll"], proxy.telemetry["pitch"])
	}
}
