import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.6
import QtPositioning 5.6

MapQuickItem {
	id: missionPoint
	property int number: 0

	Component.onCompleted: {
		canvas.requestPaint()
	}

	sourceItem: Item {
		id: control
		visible: true
		width: 30
		height: 30

		ToolTip.delay: 300
		ToolTip.visible: mouseArea.containsMouse
		ToolTip.text: "Alt: " + coordinate.altitude.toFixed(1)

		MouseArea {
			id: mouseArea
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
		}

		Canvas {
			id: canvas
			anchors.fill: parent

			onPaint: {
				var context = getContext("2d");
				context.strokeStyle = "red"
				context.fillStyle = "green";
				context.lineWidth = 3;
				context.beginPath();
				context.ellipse(3, 3, 23, 23)
				context.stroke();
				context.fill();
				context.closePath();

				context.beginPath();
				context.font = '10px sans-serif';
				context.lineWidth = 1;
				context.strokeStyle = "white"
				if (number < 10)
					context.strokeText(number, 11, 18);
				else if (number < 100)
					context.strokeText(number, 8, 18);
				else
					context.strokeText(number, 5, 18);
				context.stroke();
				context.closePath();
			}
		}
	}
}
