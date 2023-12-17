import QtQuick 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
	id: settingsWindow
	visible: false
	title: "Settings"
	width: 400
	height: 300
	modality: Qt.WindowModal
	flags: Qt.Dialog

	property alias cameraWidth: widthText.value
	property alias cameraHeight: heightText.value
	property alias cameraQuality: qualityText.value
	property alias cameraBrightness: brightnessText.value

	Component.onCompleted: {
		load()
	}

	Rectangle {
		id: tabRect
		width: parent.width
		height: parent.height - 40

		TabBar {
			id: tabBar
			width: parent.width

			TabButton {
				text: "Camera"
			}
			TabButton {
				text: "Speaker"
			}
			TabButton {
				text: "Microphone"
			}
		}

		StackLayout {
			width: parent.width
			currentIndex: tabBar.currentIndex

			Item {
				id: cameraTab

				Grid {
					id: grid
					x: 20
					y: 60
					rows: 4
					columns: 2
					spacing: 10
					anchors.margins: 10

					Text {
						text: "Width"
						font.pixelSize: 14
					}

					SpinButton2 {
						id: widthText
						value: 640
					}

					Text {
						text: "Height"
						font.pixelSize: 14
					}

					SpinButton2 {
						id: heightText
						value: 480
					}

					Text {
						text: "Quality"
						font.pixelSize: 14
					}

					SpinButton2 {
						id: qualityText
						to: 100
						value: 50
					}

					Text {
						text: "Brightness"
						font.pixelSize: 14
					}

					SpinButton2 {
						id: brightnessText
						to: 100
						value: 50
					}
				}
			}
			Item {
				id: speakerTab
				Label {
					x: 100
					y: 100
					text: "Lorem ipsum..."
				}
			}
			Item {
				id: microphoneTab
				Label {
					x: 100
					y: 100
					text: "Lorem ipsum..."
				}
			}
		}
	}

	Button {
		id: buttonSave
		x: parent.width - 213
		y: parent.height - 37
		width: 100
		height: 27
		text: "Save"

		onClicked: {
			save()
		}
	}

	Button {
		id: buttonCancel
		x: parent.width - 110
		y: parent.height - 37
		width: 100
		height: 27
		text: "Cancel"

		onClicked: {
			settingsWindow.close()
		}
	}

	function load()
	{
		cameraWidth = proxy.parameters["cameraWidth"]
		cameraHeight = proxy.parameters["cameraHeight"]
		cameraQuality = proxy.parameters["cameraQuality"]
		cameraBrightness = proxy.parameters["cameraBrightness"]
	}

	function save()
	{
		mainWindow.saveParameters()
		settingsWindow.close()
	}

}
