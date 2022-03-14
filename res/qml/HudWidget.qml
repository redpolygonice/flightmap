import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
	id: control
	visible: true
	width: 150
	height: 150
	color: "#000000"
	border.width: 2
	border.color: "#ffffff"

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		drag.target: parent
	}

	Component.onCompleted: {
		update(-30.0, -30.0)
	}

	Canvas {
		id: canvas
		anchors.fill: parent

		property real rad: Math.PI / 180
		property real x0: width / 2
		property real y0: height / 2
		property real radius: width / 2
		property real rollStartAngle: 30 * rad
		property real rollEndAngle: 150 * rad
		property real degree15: 15 * rad
		property int margin: 10
		property variant circleRect: [ margin, margin, width - (margin * 2), height - (margin * 2) ]
		property variant pitchScale: [ "30", "15", "0", "15", "30", "45" ]
		property variant rollScale: [ "60", "45", "30", "15", "0", "15", "30", "45", "60" ]

		property real roll: 0.0 // Крен
		property real pitch: 0.0 // Тангаж

		onPaint: {
			var context = getContext("2d");

			// Circle
			context.strokeStyle = "white"
			context.lineWidth = 2;
			context.beginPath();
			context.ellipse(circleRect[0], circleRect[1], circleRect[2], circleRect[3])
			context.stroke();

			// Land
			context.beginPath();
			context.fillStyle = "tan";
			context.arc(x0, y0, radius - margin, (Math.PI * 2) - pitch - roll, Math.PI + pitch - roll, false);
			context.fill();
			context.closePath();

			// Sky
			context.beginPath();
			context.fillStyle = "skyblue";
			context.arc(x0, y0, radius - margin, Math.PI + pitch - roll, (Math.PI * 2) - pitch - roll, false);
			context.fill();
			context.closePath();

			var cos = 0.0;
			var sin = 0.0;
			var x1 = 0.0;
			var y1 = 0.0;
			var x2 = 0.0;
			var y2 = 0.0;

			// Pitch scale
			var xoffset = 10;
			var yoffset = 16;
			var fontOffset = 8;
			context.strokeStyle = "white"
			context.lineWidth = 2;
			context.font = '10px sans-serif';
			context.beginPath();

			for (var i = -2; i <= 3; ++i)
			{
				x1 = radius - (xoffset + fontOffset);
				y1 = radius + (yoffset * i);
				x2 = x1 + xoffset;
				y2 = y1;

				context.lineWidth = 2;
				context.moveTo(x1, y1);
				context.lineTo(x2, y2);

				x1 = radius + fontOffset;
				y1 = radius + (yoffset * i);
				x2 = x1 + xoffset;
				y2 = y1;

				context.lineWidth = 2;
				context.moveTo(x1, y1);
				context.lineTo(x2, y2);
				context.stroke();

				if (i != 0)
				{
					context.lineWidth = 1;
					context.strokeText(pitchScale[i + 2], radius - fontOffset + 1, y1 + 4);
					context.stroke();
				}
			}

			// Roll scale
			context.strokeStyle = "white"
			context.lineWidth = 2;
			context.beginPath();

			for (var angle = rollStartAngle; angle <= rollEndAngle; angle += degree15)
			{
				cos = Math.cos(angle - roll);
				sin = Math.sin(angle - roll);
				x1 = x0 - cos * (radius - 23);
				y1 = y0 - sin * (radius - 23);
				x2 = x0 - cos * (radius - 13);
				y2 = y0 - sin * (radius - 13);
				context.moveTo(x1, y1);
				context.lineTo(x2, y2);
			}

			context.stroke();

			// Roll pointer
			context.strokeStyle = "white"
			context.lineWidth = 5;
			context.beginPath();

			x1 = x0;
			y1 = radius - 43;
			x2 = x0;
			y2 = y1 - 10;
			context.moveTo(x1, y1);
			context.lineTo(x2, y2);
			context.stroke();

			// Draw plane
			context.strokeStyle = "red"
			context.lineWidth = 2.5;
			context.beginPath();
			context.moveTo(15, y0);
			context.lineTo(48, y0);
			context.moveTo(x0, y0);
			context.lineTo(x0 - 15, y0 + 5);
			context.moveTo(x0 - 15, y0 + 5);
			context.lineTo(48, y0);
			context.moveTo(x0, y0);
			context.lineTo(x0 + 15, y0 + 5);
			context.moveTo(x0 + 15, y0 + 5);
			context.lineTo(width - 48, y0);
			context.moveTo(width - 48, y0);
			context.lineTo(width - 15, y0);
			context.stroke();
		}
	}

	function update(roll_, pitch_)
	{
		canvas.roll = roll_ * canvas.rad
		canvas.pitch = -pitch_ * canvas.rad
		canvas.requestPaint()
	}
}
