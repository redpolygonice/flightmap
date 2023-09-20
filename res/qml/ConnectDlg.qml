import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

Dialog {
	visible: false
	title: "Connection"
	width: 200
	height: 250

	property int retcode: 0
	property string protocol: ""
	property string device: ""
	property string host: ""
	property int port: 0
	property int baudrate: 0

	Component.onCompleted: {
		protocolComboBox.model = proxy.protocols
		deviceComboBox.model = proxy.devices
		protocol = proxy.connection["protocol"]
		device = proxy.connection["device"]
		host = proxy.connection["host"]
		port = proxy.connection["port"]
		baudrate = proxy.connection["baudrate"]
		update()
	}

	Grid {
		id: grid
		anchors.fill: parent
		rows: 5
		columns: 2
		spacing: 10
		anchors.margins: 10

		Text {
			id: text1
			text: qsTr("Protocol")
			font.pixelSize: 12
		}

		ComboBox {
			id: protocolComboBox
			width: 100
			onCurrentIndexChanged: {
				if (currentIndex == 3)
				{
					deviceComboBox.enabled = false
					baudrateText.enabled = false
					hostText.enabled = false
					portText.enabled = false
				}
				else if (currentIndex <= 2)
				{
					deviceComboBox.enabled = true
					baudrateText.enabled = false
					hostText.enabled = true
					portText.enabled = true
				}
				else
				{
					deviceComboBox.enabled = true
					baudrateText.enabled = true
					hostText.enabled = false
					portText.enabled = false
				}
			}
		}

		Text {
			id: text2
			text: qsTr("Device")
			font.pixelSize: 12
		}

		ComboBox {
			id: deviceComboBox
			width: 100
		}

		Text {
			id: text3
			text: qsTr("Host")
			font.pixelSize: 12
		}

		TextField {
			id: hostText
			text: host
		}

		Text {
			id: text4
			text: qsTr("Port")
			font.pixelSize: 12
		}

		TextField {
			id: portText
			text: port
		}

		Text {
			id: text5
			text: qsTr("Baudrate")
			font.pixelSize: 12
		}

		TextField {
			id: baudrateText
			text: baudrate
		}
	}

	standardButtons: StandardButton.Ok | StandardButton.Cancel

	onAccepted: {
		retcode = 1
		protocol = protocolComboBox.currentText
		device = deviceComboBox.currentText
		host = hostText.text
		port = portText.text
		baudrate = baudrateText.text

		proxy.connection = { "protocol": protocol, "device": device, "host": host,
			"port": port, "baudrate": baudrate }
		proxy.connect()
	}

	onRejected: {
		retcode = 0
	}

	function update()
	{
		var index = protocolComboBox.find(protocol)
		protocolComboBox.currentIndex = index === -1 ? 0 : index
		index = deviceComboBox.find(device)
		deviceComboBox.currentIndex = index === -1 ? 0 : index
		hostText.text = host
		portText.text = port
		baudrateText.text = baudrate
	}
}
