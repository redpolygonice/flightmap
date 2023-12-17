import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

CheckBox {
	id: control
	Layout.leftMargin: 5

	indicator: Rectangle {
		implicitWidth: 16
		implicitHeight: 16
		x: control.leftPadding
		y: parent.height / 2 - height / 2
		border.color: control.down ? "dark" : "grey"
		Rectangle {
			anchors.fill: parent
			visible: control.checked
			color: enabled ? "#555" : "grey"
			anchors.margins: 4
		}
	}

	contentItem: Text {
		text: control.text
		color: enabled ? "white" : "grey"
		verticalAlignment: Text.AlignVCenter
		leftPadding: 22
		font.pixelSize: 12
	}
}
