import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
	id: control
	visible: true
	width: 285
	height: 140
	color: "#000000"
	border.width: 2
	border.color: "#ffffff"

	property int pixelSize: 12

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		drag.target: parent
	}

	RowLayout {
		anchors.fill: parent
		spacing: 5
		anchors.margins: 10

		ColumnLayout {

			Grid {
				id: grid
				rows: 3
				columns: 3
				spacing: 5
				anchors.margins: 10

				Rectangle {
					width: 60
					height: 20
					color: "#000000"
				}

				Button {
					id: buttonForward
					width: 60
					height: 20
					text: qsTr("Forward")
					font.pixelSize: pixelSize
				}

				Rectangle {
					width: 60
					height: 20
					color: "#000000"
				}

				Button {
					id: buttonLeft
					width: 60
					height: 20
					text: qsTr("Left")
					font.pixelSize: pixelSize
				}

				Button {
					id: buttonBeep
					width: 60
					height: 20
					text: qsTr("Beep")
					font.pixelSize: pixelSize
				}

				Button {
					id: buttonRight
					width: 60
					height: 20
					text: qsTr("Right")
					font.pixelSize: pixelSize
				}

				Button {
					id: buttonTurnLeft
					width: 60
					height: 20
					text: qsTr("Turn L")
					font.pixelSize: pixelSize
				}

				Button {
					id: buttonBack
					width: 60
					height: 20
					text: qsTr("Back")
					font.pixelSize: pixelSize
				}

				Button {
					id: buttonTurnRight
					width: 60
					height: 20
					text: qsTr("Turn R")
					font.pixelSize: pixelSize
				}
			}

			Grid {
				rows: 2
				columns: 1
				spacing: 5
				anchors.margins: 10

				Button {
					id: buttonUp
					width: grid.width
					height: 20
					text: qsTr("Up")
					font.pixelSize: pixelSize
				}

				Button {
					id: buttonDown
					width: grid.width
					height: 20
					text: qsTr("Down")
					font.pixelSize: pixelSize
				}
			}
		}

		Grid {
			rows: 5
			columns: 1
			spacing: 5
			anchors.margins: 10

			Button {
				id: buttonArm
				width: 70
				height: 20
				text: qsTr("Arm")
				font.pixelSize: pixelSize
			}

			Button {
				id: buttonLoiter
				width: 70
				height: 20
				text: qsTr("Loiter")
				font.pixelSize: pixelSize
			}

			Button {
				id: buttonAuto
				width: 70
				height: 20
				text: qsTr("Auto")
				font.pixelSize: pixelSize
			}

			Button {
				id: buttonDisarm
				width: 70
				height: 20
				text: qsTr("Disarm")
				font.pixelSize: pixelSize
			}

			Button {
				id: buttonRtl
				width: 70
				height: 20
				text: qsTr("RTL")
				font.pixelSize: pixelSize

				contentItem: Text {
					text: buttonRtl.text
					font: buttonRtl.font
					color: "white"
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					elide: Text.ElideRight
				}

				background: Rectangle {
					color: buttonRtl.pressed ? "red" : "maroon"
					border.color: "white"
					border.width: 2
				}
			}
		}
	}
}
