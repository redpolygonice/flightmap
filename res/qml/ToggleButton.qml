import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

RadioButton {
	id: control
	Layout.leftMargin: 5

	indicator: Rectangle {
		implicitWidth: 16
		implicitHeight: 16
		radius: 9
		border.color: control.activeFocus ? "black" : "gray"
		border.width: 1
		Rectangle {
			anchors.fill: parent
			visible: control.checked
			color: "#555"
			radius: 9
			anchors.margins: 4
		}
	}

	contentItem: Text {
		text: control.text
		color: "white"
		verticalAlignment: Text.AlignVCenter
		leftPadding: 20
		font.pixelSize: 12
	}
}
