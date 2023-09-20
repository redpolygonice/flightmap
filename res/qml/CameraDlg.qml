import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.15

Dialog {
	visible: false
	title: "Camera"
	width: 400
	height: 400
	modality: "NonModal"
	standardButtons: StandardButton.Close

	onRejected: {
		stopCamera()
	}

	onVisibleChanged: {
		if (visible)
		{
			contentItem.ApplicationWindow.window.flags |= Qt.WindowStaysOnTopHint
			startCamera()
		}
		else
			stopCamera()
	}

	Image {
		id: imageId
		visible: true
		anchors.fill: parent
		fillMode: Image.PreserveAspectFit
		antialiasing: true
		cache: false
		source: ""
	}

	Timer {
		id: timerId
		interval: 50
		repeat: true
		running: false
		onTriggered: {
			imageId.source = "";
			imageId.source = "file://" + proxy.imageLocation()
		}
	}

	function startCamera()
	{
		timerId.start()
		proxy.startCamera()
	}

	function stopCamera()
	{
		timerId.stop()
		proxy.stopCamera()
	}
}
