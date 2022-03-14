import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
	id: control
	visible: true
	width: 200
	height: 323
	color: "#000000"
	border.width: 2
	border.color: "#ffffff"

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		drag.target: parent
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 7
		anchors.margins: 10

		RowLayout {
			spacing: 7
			width: parent.width
			height: 20
			Layout.alignment: Qt.AlignHCenter

			Button {
				id: buttonStart
				Layout.preferredWidth: 50
				Layout.preferredHeight: 20
				text: qsTr("Start")
				font.pixelSize: 12

				onClicked: {
					mapId.isMission = true
				}
			}

			Button {
				id: buttonStop
				Layout.preferredWidth: 50
				Layout.preferredHeight: 20
				text: qsTr("Stop")
				font.pixelSize: 12

				onClicked: {
					mapId.isMission = false
				}
			}

			Button {
				id: buttonClearAll
				Layout.preferredWidth: 50
				Layout.preferredHeight: 20
				text: qsTr("Clear")
				font.pixelSize: 12

				onClicked: {
					mapId.clearMapItems()
					mapId.isMission = false
				}
			}
		}

		ColumnLayout {
			width: parent.width
			spacing: 0

			Rectangle {
				Layout.preferredWidth: parent.width
				Layout.preferredHeight: 13
				color: "#ffffff"
				Layout.alignment: Qt.AlignHCenter
				Text {
					anchors.fill: parent
					horizontalAlignment: Text.AlignHCenter
					text: qsTr("Draw mode")
					font.pixelSize: 10
				}
			}

			Rectangle {
				Layout.preferredWidth: parent.width
				Layout.preferredHeight: 40
				color: "#000000"
				border.width: 1
				border.color: "#ffffff"

				ColumnLayout {
					width: parent.width
					height: parent.height
					spacing: 3

					ToggleButton {
						id: drawModePolygon
						Layout.topMargin: 3
						Layout.preferredWidth: parent.width
						Layout.preferredHeight: 15
						checked: true
						text: qsTr("Polygon")
					}
					ToggleButton {
						id: drawModePoints
						Layout.preferredWidth: parent.width
						Layout.preferredHeight: 15
						text: qsTr("Points")
					}
					Rectangle {
						Layout.fillHeight: true
					}
				}
			}
		}

		ColumnLayout {
			width: parent.width
			spacing: 0

			Rectangle {
				Layout.preferredWidth: parent.width
				Layout.preferredHeight: 13
				color: "#ffffff"
				Layout.alignment: Qt.AlignHCenter
				Text {
					anchors.fill: parent
					horizontalAlignment: Text.AlignHCenter
					text: qsTr("Mission grid")
					font.pixelSize: 10
				}
			}

			Rectangle {
				Layout.preferredWidth: parent.width
				Layout.preferredHeight: 150
				color: "#000000"
				border.width: 1
				border.color: "#ffffff"

				ColumnLayout {
					width: parent.width
					height: parent.height
					spacing: 7
					Layout.alignment: Qt.AlignVCenter

					RowLayout {
						Layout.preferredHeight: 15
						Layout.preferredWidth: parent.width
						Layout.alignment: Qt.AlignVCenter
						Layout.topMargin: 7
						Layout.rightMargin: 3

						CheckButton {
							id: horizStepCheck
							Layout.preferredHeight: 15
							text: "Horiz step"

							onCheckedChanged: {
								if (checked)
									horizStepValue.enabled = true
								else
									horizStepValue.enabled = false
							}

							Component.onCompleted: {
								onCheckedChanged()
							}
						}

						SpinButton {
							id: horizStepValue
							Layout.preferredHeight: 15
							Layout.preferredWidth: 60
						}
					}

					RowLayout {
						Layout.preferredHeight: 15
						Layout.preferredWidth: parent.width
						Layout.alignment: Qt.AlignVCenter

						CheckButton {
							id: vertStepCheck
							Layout.preferredHeight: 15
							text: "Vert step"

							onCheckedChanged: {
								if (checked)
									vertStepValue.enabled = true
								else
									vertStepValue.enabled = false
							}

							Component.onCompleted: {
								onCheckedChanged()
							}
						}

						SpinButton {
							id: vertStepValue
							Layout.preferredHeight: 15
							Layout.preferredWidth: 60
						}
					}

					RowLayout {
						Layout.preferredHeight: 15
						Layout.preferredWidth: parent.width
						Layout.alignment: Qt.AlignVCenter

						Label {
							Layout.preferredHeight: 15
							text: "Points at every"
							color: "white"
							Layout.leftMargin: 10
							font.pixelSize: 12
						}

						SpinButton {
							id: missionStepValue
							Layout.preferredHeight: 15
							Layout.preferredWidth: 60
						}

					}

					RowLayout {
						Layout.preferredHeight: 15
						Layout.preferredWidth: parent.width
						Layout.alignment: Qt.AlignVCenter

						Label {
							Layout.preferredHeight: 15
							text: "Alt"
							color: "white"
							Layout.leftMargin: 10
							font.pixelSize: 12
						}

						SpinButton {
							id: altValue
							Layout.preferredHeight: 15
							Layout.preferredWidth: 60
						}

						ComboBox {
							id: altType
							width: 60
							Layout.preferredHeight: 20
							Layout.preferredWidth: 60
							model: [ "Absolut", "Relat", "Ground" ]
						}
					}

					RowLayout {
						Layout.preferredHeight: 15
						Layout.preferredWidth: parent.width
						Layout.alignment: Qt.AlignVCenter

						CheckButton {
							id: checkAlt
							Layout.preferredHeight: 15
							text: "Check altitude"
						}
					}

					RowLayout {
						spacing: 7
						width: parent.width
						height: 20
						Layout.alignment: Qt.AlignHCenter

						Button {
							id: buttonCreate
							Layout.preferredWidth: 70
							Layout.preferredHeight: 20
							text: qsTr("Create")
							font.pixelSize: 12
						}

						Button {
							id: buttonClear
							Layout.preferredWidth: 70
							Layout.preferredHeight: 20
							text: qsTr("Clear")
							font.pixelSize: 12
						}
					}

					Rectangle {
						Layout.fillHeight: true
					}
				}
			}
		}

		ColumnLayout {
			width: parent.width
			spacing: 0

			Rectangle {
				Layout.preferredWidth: parent.width
				Layout.preferredHeight: 13
				color: "#ffffff"
				Layout.alignment: Qt.AlignHCenter
				Text {
					anchors.fill: parent
					horizontalAlignment: Text.AlignHCenter
					text: qsTr("Restore mission")
					font.pixelSize: 10
				}
			}

			Rectangle {
				Layout.preferredWidth: parent.width
				Layout.preferredHeight: 36
				color: "#000000"
				border.width: 1
				border.color: "#ffffff"

				ColumnLayout {
					width: parent.width
					height: parent.height
					spacing: 7
					Layout.alignment: Qt.AlignVCenter

					RowLayout {
						spacing: 7
						width: parent.width
						height: 20
						Layout.alignment: Qt.AlignHCenter

						Button {
							id: buttonRestore
							Layout.preferredWidth: 70
							Layout.preferredHeight: 20
							text: qsTr("Restore")
							font.pixelSize: 12
						}

						Button {
							id: buttonWrite
							Layout.preferredWidth: 70
							Layout.preferredHeight: 20
							text: qsTr("Write")
							font.pixelSize: 12
						}
					}
				}
			}
		}
	}
}
