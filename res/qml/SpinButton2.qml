import QtQuick 2.15
import QtQuick.Controls 2.15

SpinBox {
	id: control
	from: 10
	to: 9999
	stepSize: 10
	height: 25
	width: 140
	editable: true
	font.pixelSize: 14
}
