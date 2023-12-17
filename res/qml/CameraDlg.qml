import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.15

Dialog {
	visible: false
	title: "Camera"
	width: 600
	height: 500
	modality: Qt.NonModal
	standardButtons: StandardButton.Close

	property string imagePath: "file:///" + proxy.imageLocation()

	onRejected: {
		stopCamera()
	}

	onVisibleChanged: {
		if (visible)
		{
			contentItem.ApplicationWindow.window.flags |= Qt.Widget
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
			if (proxy.imageExists())
			{
				imageId.source = "";
				imageId.source = imagePath
			}
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
