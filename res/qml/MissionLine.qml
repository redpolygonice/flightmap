import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.6
import QtPositioning 5.6

MapQuickItem {
	id: missionLine

	Component.onCompleted: {
		canvas.requestPaint()
	}

	sourceItem: Item {
		id: control
		visible: true
		width: 30
		height: 30

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
			}
		}
	}
}
