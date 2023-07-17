import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

Button {
	id: control
	ToolTip.visible: hovered
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	padding: 0
	font.pixelSize: 12

	Timer
	{
		id: timer
		triggeredOnStart: true
		repeat: true
	}

	function startTimer(delayTime, cb)
	{
		timer.interval = delayTime;
		timer.triggered.connect(cb);
		timer.start();
	}

	function stopTimer() {
		timer.stop()
	}

	contentItem: Text {
		text: control.text
		font: control.font
		color: {
			return control.checked ? "red" : "black"
		}
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
	}

	background: Rectangle {

		color: {
			if (mouseArea.containsMouse)
				return "#aaaaaa"
			else if (control.checked || control.down)
				return "#aaaaaa"
			else
				return "#e0e0e0"
		}

		MouseArea {
			id: mouseArea
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: control.clicked()
			onPressed: control.onPressed()
			onReleased: control.onReleased()
			onPressAndHold: control.onPressAndHold()
		}
	}
}
