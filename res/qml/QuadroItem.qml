import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtLocation 5.6
import QtPositioning 5.6

MapQuickItem {
	id: quadroItem

	property bool active: false
	property int number: 1
	property string name: "Quadrocopter"
	property real angle: 0

	Component.onCompleted: {
		canvas.requestPaint()
	}

	sourceItem: Image {
		id: image
		visible: true
		width: 42
		height: 42
		fillMode: Image.Stretch
		source: "qrc:/img/quadro.png";
		transform: Rotation { origin.x: image.width / 2; origin.y: image.height / 2; angle: angle }

		ToolTip.delay: 300
		ToolTip.visible: mouseArea.containsMouse
		ToolTip.text: name + " #" + number

		MouseArea {
			id: mouseArea
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor

			onClicked: {
				active = true
				mapId.setActiveQuadro(number)
				proxy.setAcive(number)
			}
		}

		Canvas {
			id: canvas
			anchors.fill: parent
			property real rad: Math.PI / 180

			onPaint: {
				var context = getContext("2d");
				context.font = '9px sans-serif';
				context.lineWidth = 1;
				if (active)
					context.strokeStyle = "white"
				else
					context.strokeStyle = "grey"
				context.strokeText(number, image.width / 2 - 3, image.height / 2 + 3);
				context.stroke();
			}
		}
	}

	function repaint()
	{
		canvas.requestPaint()
	}

	function update(yaw)
	{
		angle = yaw
		canvas.requestPaint()
		image.transform[0].angle = angle
	}
}
