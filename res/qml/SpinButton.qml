import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.15

SpinBox {
    id: control
    from: 5
    to: 200
    value: 50
    stepSize: 5
    height: 16
    editable: true
    font.pixelSize: 13

    contentItem: TextInput {
        text: control.textFromValue(control.value, control.locale)
        font: control.font
        leftPadding: -7
        bottomPadding: -5
        rightPadding: 5
        color: "black"
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }

    up.indicator: Rectangle {
        x: control.mirrored ? 0 : parent.width - width
        height: parent.height
        implicitWidth: 16
        implicitHeight: 16
        color: control.up.pressed ? "#e4e4e4" : "#f6f6f6"
        border.color: enabled ? "#21be2b" : "#bdbebf"

        Text {
            text: "+"
            font.pixelSize: control.font.pixelSize
            color: enabled ? "black" : "grey"
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    down.indicator: Rectangle {
        x: control.mirrored ? parent.width - width : 0
        height: parent.height
        implicitWidth: 16
        implicitHeight: 16
        color: control.down.pressed ? "#e4e4e4" : "#f6f6f6"
        border.color: enabled ? "#21be2b" : "#bdbebf"

        Text {
            text: "-"
            font.pixelSize: control.font.pixelSize
            color: enabled ? "black" : "grey"
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
