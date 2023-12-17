import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
	id: control
	visible: true
	width: 285
	height: 140
	color: backColor
	border.width: 2
	border.color: "#ffffff"
	radius: 10

	property int pixelSize: 12
	property color backColor: "#172839"

	property int channelLow: 1100
	property int channelHigh: 1900
	property int defaultChannel: 1500

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
					color: backColor
				}

				SimpleButton {
					id: buttonForward
					width: 60
					height: 20
					text: qsTr("Forward")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Forward")

					onPressed: {
						proxy.moveForward()
					}

					onReleased: {
						stopTimer()
						proxy.moveForward(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.moveForward(false)
						})
					}
				}

				Rectangle {
					width: 60
					height: 20
					color: backColor
				}

				SimpleButton {
					id: buttonLeft
					width: 60
					height: 20
					text: qsTr("Left")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Left")

					onPressed: {
						proxy.moveLeft()
					}

					onReleased: {
						stopTimer()
						proxy.moveLeft(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.moveLeft(false)
						})
					}
				}

				SimpleButton {
					id: buttonBeep
					width: 60
					height: 20
					text: qsTr("Beep")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Beep")

					onClicked: {
						proxy.beep()
					}
				}

				SimpleButton {
					id: buttonRight
					width: 60
					height: 20
					text: qsTr("Right")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Right")

					onPressed: {
						proxy.moveRight()
					}

					onReleased: {
						stopTimer()
						proxy.moveRight(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.moveRight(false)
						})
					}
				}

				SimpleButton {
					id: buttonTurnLeft
					width: 60
					height: 20
					text: qsTr("Turn L")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Turn left")

					onPressed: {
						proxy.turnLeft()
					}

					onReleased: {
						stopTimer()
						proxy.turnLeft(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.turnLeft(false)
						})
					}
				}

				SimpleButton {
					id: buttonBack
					width: 60
					height: 20
					text: qsTr("Back")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Back")

					onPressed: {
						proxy.moveBack()
					}

					onReleased: {
						stopTimer()
						proxy.moveBack(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.moveBack(false)
						})
					}
				}

				SimpleButton {
					id: buttonTurnRight
					width: 60
					height: 20
					text: qsTr("Turn R")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Turn right")

					onPressed: {
						proxy.turnRight()
					}

					onReleased: {
						stopTimer()
						proxy.turnRight(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.turnRight(false)
						})
					}
				}
			}

			Grid {
				rows: 2
				columns: 1
				spacing: 5
				anchors.margins: 10

				SimpleButton {
					id: buttonUp
					width: grid.width
					height: 20
					text: qsTr("Up")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Up")

					onPressed: {
						proxy.moveUp()
					}

					onReleased: {
						stopTimer()
						proxy.moveUp(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.moveUp(false)
						})
					}
				}

				SimpleButton {
					id: buttonDown
					width: grid.width
					height: 20
					text: qsTr("Down")
					font.pixelSize: pixelSize
					ToolTip.text: qsTr("Down")

					onPressed: {
						proxy.moveDown()
					}

					onReleased: {
						stopTimer()
						proxy.moveDown(true)
					}

					onPressAndHold: {
						startTimer(300, function()
						{
							proxy.moveDown(false)
						})
					}
				}
			}
		}

		Grid {
			rows: 5
			columns: 1
			spacing: 5
			anchors.margins: 10

			SimpleButton {
				id: buttonArm
				width: 70
				height: 20
				text: qsTr("Arm")
				font.pixelSize: pixelSize
				ToolTip.text: qsTr("Turn on engine")

				onClicked: {
					proxy.arm(true)
				}
			}

			SimpleButton {
				id: buttonLoiter
				width: 70
				height: 20
				text: qsTr("Loiter")
				font.pixelSize: pixelSize
				ToolTip.text: qsTr("Loiter mode - manual control")

				onClicked: {
					proxy.loiter()
				}
			}

			SimpleButton {
				id: buttonAuto
				width: 70
				height: 20
				text: qsTr("Auto")
				font.pixelSize: pixelSize
				ToolTip.text: qsTr("Auto mode - start mission")

				onClicked: {
					proxy.autom()
				}
			}

			SimpleButton {
				id: buttonDisarm
				width: 70
				height: 20
				text: qsTr("Disarm")
				font.pixelSize: pixelSize
				ToolTip.text: qsTr("Turn off engine")

				onClicked: {
					proxy.arm(false)
				}
			}

			Button {
				id: buttonRtl
				width: 70
				height: 20
				text: qsTr("RTL")
				font.pixelSize: pixelSize

				ToolTip.visible: hovered
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.text: qsTr("Return to launch")

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

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor

					onClicked: {
						proxy.rtl()
					}
				}
			}
		}
	}
}
