import QtQuick 2.0
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import QtLocation 5.6
import QtQuick.Controls.Styles 1.4

Dialog {
	visible: false
	title: "Maps"
	width: 370
	height: 450
	font.pixelSize: 14

	standardButtons: StandardButton.Ok | StandardButton.Cancel

	Plugin {
		id: mapPlugin
	}

	Component.onCompleted: {
		for (var i = 0; i < mapPlugin.availableServiceProviders.length; i++) {
			var provider = mapPlugin.availableServiceProviders[i]
			var plugin = Qt.createQmlObject('import QtLocation 5.6; Plugin { name:"' + provider + '"}', mainWindow)
			var map = Qt.createQmlObject('import QtLocation 5.6; Map { }', mainWindow)
			map.plugin = plugin

			for (var j = 0; j < map.supportedMapTypes.length; j++) {
				listModel.append({ provider: provider, type: map.supportedMapTypes[j].name })
			}

			map.destroy()
			plugin.destroy()
		}
	}

	Rectangle {
		anchors.fill: parent
		color: "#ffffff"
		border.width: 2
		border.color: "#aaaaaa"
		radius: 3


		ListModel {
			id: listModel
		}

		Component {
			id: mapDelegate

			Item {
				width: listView.width
				height: 22

				RowLayout {
					Layout.alignment: Qt.AlignVCenter
					Text { text: '<b> ' + provider + '</b>:'; color: "black"; Layout.alignment: Qt.AlignVCenter; font.pixelSize: 14 }
					Text { text: type; color: "dimgrey"; Layout.alignment: Qt.AlignVCenter; font.pixelSize: 14}
				}

				MouseArea {
					anchors.fill: parent
					onClicked: listView.currentIndex = model.index
					onDoubleClicked: {
						accept()
						select(model.index)
					}
				}
			}
		}

		ScrollView {
			anchors.fill: parent

			ListView {
				id: listView
				anchors.fill: parent
				anchors.margins: 1
				spacing: 5
				clip: true
				model: listModel
				delegate: mapDelegate
				highlight: Rectangle { color: "lightsteelblue"; radius: 3 }
				highlightFollowsCurrentItem: true
				highlightMoveDuration: 100
				highlightMoveVelocity: -1
				focus: true
			}
		}
	}

	onAccepted: {
		select(listView.currentIndex)
	}

	function select(index)
	{
		var provider = listModel.get(index).provider
		var type = listModel.get(index).type
		var parameters = { "provider": provider, "mapType": type }
		mainWindow.createMap(parameters)
	}
}
