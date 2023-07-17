import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.1
import Qt.labs.platform 1.1

Rectangle {
    id: control
    visible: true
    width: 200
    height: 349
    color: backColor
    border.width: 2
    border.color: "#ffffff"
    radius: 10

    property color backColor: "#172839"

    property bool polygon: false
    property alias alt: altValue
    property alias altType: altType
    property alias altCheck: checkAlt
    property alias horizStep: horizStepValue
    property alias horizStepCheck: horizStepCheck
    property alias vertStep: vertStepValue
    property alias vertStepCheck: vertStepCheck
    property alias missionStep: missionStepValue

    Component.onCompleted: {

        polygon = proxy.parameters["missionPolygon"]
        drawModePolygon.checked = polygon
        drawModePoints.checked = !polygon
        altValue.value = proxy.parameters["missionAlt"]
        altType.currentIndex = proxy.parameters["missionAltType"]
        checkAlt.checked = proxy.parameters["missionAltCheck"]
        horizStepValue.value = proxy.parameters["missionHorizStep"]
        horizStepCheck.checked = proxy.parameters["missionHorizStepCheck"]
        vertStepValue.value = proxy.parameters["missionVertStep"]
        vertStepCheck.checked = proxy.parameters["missionVertStepCheck"]
        missionStepValue.value = proxy.parameters["missionStep"]
    }

    MessageDialog {
        id: messageDialog
        visible: false
        title: ""
        text: ""
        onAccepted: {
            visible = false
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: parent
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 7
        anchors.margins: 10

        RowLayout {
            spacing: 7
            width: parent.width
            height: 20
            Layout.alignment: Qt.AlignHCenter

            SimpleButton {
                id: buttonStart
                Layout.preferredWidth: 80
                Layout.preferredHeight: 20
                text: qsTr("Start create")
                ToolTip.text: qsTr("Start create mission")
                checked: false

                onClicked: {
                    if (!checked)
                    {
                        mapId.clearAll()
                        mapId.isMission = true
                        missionCreated = false
                        text = qsTr("Stop")
                        ToolTip.text = qsTr("Stop create mission")
                        checked = true
                        proxy.log("Mission creation started!")
                    }
                    else
                    {
                        mapId.isMission = false
                        text = qsTr("Start create")
                        ToolTip.text = qsTr("Start create mission")
                        checked = false
                        proxy.log("Mission creation stopped!")
                    }
                }
            }

            SimpleButton {
                id: buttonClearAll
                Layout.preferredWidth: 80
                Layout.preferredHeight: 20
                text: qsTr("Clear all")
                ToolTip.text: qsTr("Clear all items")

                onClicked: {
                    mapId.clearAll()
                    mapId.isMission = false
                    mapId.missionCreated = false
                    buttonStart.checked = false
                    buttonStart.text = qsTr("Start create")
                    buttonStart.ToolTip.text = qsTr("Start mission")
                }
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 0

            Rectangle {
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 13
                color: "#ffffff"
                Layout.alignment: Qt.AlignHCenter
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Draw mode")
                    font.pixelSize: 10
                }
            }

            Rectangle {
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 40
                color: backColor
                border.width: 1
                border.color: "#ffffff"

                ColumnLayout {
                    width: parent.width
                    height: parent.height
                    spacing: 3

                    ToggleButton {
                        id: drawModePoints
                        Layout.topMargin: 3
                        Layout.preferredWidth: parent.width
                        Layout.preferredHeight: 15
                        text: qsTr("Points")

                        onCheckedChanged: {
                            if (checked)
                            {
                                polygon = false
                                horizStepCheck.enabled = false
                                horizStepValue.enabled = false
                                vertStepCheck.enabled = false
                                vertStepValue.enabled = false
                            }
                        }
                    }

                    ToggleButton {
                        id: drawModePolygon
                        Layout.preferredWidth: parent.width
                        Layout.preferredHeight: 15
                        text: qsTr("Polygon")

                        onCheckedChanged: {
                            if (checked)
                            {
                                polygon = true
                                horizStepCheck.enabled = true
                                horizStepValue.enabled = true
                                vertStepCheck.enabled = true
                                vertStepValue.enabled = true
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
                    }
                }
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 0

            Rectangle {
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 13
                color: "#ffffff"
                Layout.alignment: Qt.AlignHCenter
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Mission grid")
                    font.pixelSize: 10
                }
            }

            Rectangle {
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 150
                color: backColor
                border.width: 1
                border.color: "#ffffff"

                ColumnLayout {
                    width: parent.width
                    height: parent.height
                    spacing: 7
                    Layout.alignment: Qt.AlignVCenter

                    RowLayout {
                        Layout.preferredHeight: 15
                        Layout.preferredWidth: parent.width
                        Layout.alignment: Qt.AlignVCenter
                        Layout.topMargin: 7
                        Layout.rightMargin: 3

                        CheckButton {
                            id: horizStepCheck
                            Layout.preferredHeight: 15
                            text: "Horiz step"

                            onCheckedChanged: {
                                if (checked && drawModePolygon.checked)
                                    horizStepValue.enabled = true
                                else
                                    horizStepValue.enabled = false
                            }

                            Component.onCompleted: {
                                onCheckedChanged()
                            }
                        }

                        SpinButton {
                            id: horizStepValue
                            Layout.preferredHeight: 15
                            Layout.preferredWidth: 60
                        }
                    }

                    RowLayout {
                        Layout.preferredHeight: 15
                        Layout.preferredWidth: parent.width
                        Layout.alignment: Qt.AlignVCenter

                        CheckButton {
                            id: vertStepCheck
                            Layout.preferredHeight: 15
                            text: "Vert step"

                            onCheckedChanged: {
                                if (checked && drawModePolygon.checked)
                                    vertStepValue.enabled = true
                                else
                                    vertStepValue.enabled = false
                            }

                            Component.onCompleted: {
                                onCheckedChanged()
                            }
                        }

                        SpinButton {
                            id: vertStepValue
                            Layout.preferredHeight: 15
                            Layout.preferredWidth: 60
                        }
                    }

                    RowLayout {
                        Layout.preferredHeight: 15
                        Layout.preferredWidth: parent.width
                        Layout.alignment: Qt.AlignVCenter

                        Label {
                            Layout.preferredHeight: 15
                            text: "Control points"
                            color: "white"
                            Layout.leftMargin: 10
                            font.pixelSize: 12
                        }

                        SpinButton {
                            id: missionStepValue
                            Layout.preferredHeight: 15
                            Layout.preferredWidth: 60
                        }

                    }

                    RowLayout {
                        Layout.preferredHeight: 15
                        Layout.preferredWidth: parent.width
                        Layout.alignment: Qt.AlignVCenter

                        Label {
                            Layout.preferredHeight: 15
                            text: "Alt"
                            color: "white"
                            Layout.leftMargin: 10
                            font.pixelSize: 12
                        }

                        SpinButton {
                            id: altValue
                            Layout.preferredHeight: 15
                            Layout.preferredWidth: 60
                        }

                        ComboBox {
                            id: altType
                            width: 60
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 60
                            model: [ "Absolute", "Relative", "Terrain" ]
                        }
                    }

                    RowLayout {
                        Layout.preferredHeight: 15
                        Layout.preferredWidth: parent.width
                        Layout.alignment: Qt.AlignVCenter

                        CheckButton {
                            id: checkAlt
                            Layout.preferredHeight: 15
                            text: "Check altitude"
                            checked: true
                        }
                    }

                    RowLayout {
                        spacing: 7
                        width: parent.width
                        height: 20
                        Layout.alignment: Qt.AlignHCenter

                        SimpleButton {
                            id: buttonCreate
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 20
                            text: qsTr("Create")
                            ToolTip.text: qsTr("Create mission points")

                            onClicked: {
                                if (mapId.missionCreated)
                                {
                                    proxy.log("Mission already created!")
                                    return;
                                }

                                if (mapId.createMission())
                                {
                                    mapId.missionCreated = true
                                    mapId.isMission = false
                                    buttonStart.checked = false
                                    buttonStart.text = qsTr("Start create")
                                    buttonStart.ToolTip.text = qsTr("Start mission")
                                }
                            }
                        }

                        SimpleButton {
                            id: buttonClear
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 20
                            text: qsTr("Clear")
                            ToolTip.text: qsTr("Clear mission points")

                            onClicked: {
                                mapId.clearMission()
                                mapId.missionCreated = false
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
                    }
                }
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 0

            Rectangle {
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 13
                color: "#ffffff"
                Layout.alignment: Qt.AlignHCenter
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Restore mission")
                    font.pixelSize: 10
                }
            }

            Rectangle {
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 61
                color: backColor
                border.width: 1
                border.color: "#ffffff"

                ColumnLayout {
                    width: parent.width
                    height: parent.height
                    spacing: 5
                    Layout.alignment: Qt.AlignVCenter

                    RowLayout {
                        spacing: 7
                        width: parent.width
                        height: 20
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 8

                        SimpleButton {
                            id: buttonRestore
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 20
                            text: qsTr("Restore")
                            ToolTip.text: qsTr("Restore mission from device")

                            onClicked: {
                                proxy.readMission()
                                mapId.missionCreated = true
                            }
                        }

                        SimpleButton {
                            id: buttonWrite
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 20
                            text: qsTr("Write")
                            ToolTip.text: qsTr("Write mission to device")

                            onClicked: {
                                if (!missionCreated)
                                {
                                    messageDialog.text = "You must first create mission!"
                                    messageDialog.open()
                                }
                                else
                                    proxy.writeMission()
                            }
                        }
                    }

                    RowLayout {
                        spacing: 7
                        width: parent.width
                        height: 20
                        Layout.alignment: Qt.AlignHCenter

                        SimpleButton {
                            id: buttonLoad
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 20
                            text: qsTr("Load")
                            ToolTip.text: qsTr("Load mission from file")

                            onClicked: {
                                fileLoadDialog.open()
                            }
                        }

                        SimpleButton {
                            id: buttonSave
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 20
                            text: qsTr("Save")
                            ToolTip.text: qsTr("Save mission to file")

                            onClicked: {
                                if (!missionCreated)
                                {
                                    messageDialog.text = "You must first create mission!"
                                    messageDialog.open()
                                }
                                else
                                    fileSaveDialog.open()
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileSaveDialog
        title: qsTr("Save mission as ..")
        fileMode: FileDialog.SaveFile
        nameFilters: [ "Mission files (*.fmm)" ]
        selectedNameFilter.index: 1
        currentFile: {
            var path = proxy.parameters["missionFilePath"]
            if (path === undefined)
                return StandardPaths.writableLocation(StandardPaths.HomeLocation) + "//mission.fmm"
            else
                return "file:////" + path + "//mission.fmm"
        }

        onAccepted: {
            var fileName = file.toString().replace("file://", "")
            proxy.saveMissionToFile(fileName, getParams())
        }
    }

    FileDialog {
        id: fileLoadDialog
        title: qsTr("Open mission file")
        fileMode: FileDialog.OpenFile
        nameFilters: [ "Mission files (*.fmm)" ]
        selectedNameFilter.index: 1
        folder: {
            var path = proxy.parameters["missionFilePath"]
            if (path === undefined)
                return StandardPaths.writableLocation(StandardPaths.HomeLocation)
            else
                return  "file:////" + path
        }

        onAccepted: {
            var fileName = file.toString().replace("file://", "")
            var params = proxy.loadMissionFromFile(fileName)
            if (params.length === 0)
                return

            setParams(params)
            mapId.drawMission()
            missionCreated = true
        }
    }

    function getParams()
    {
        var params = { "polygon": polygon,
            "homeLat": mapId.homePoint.latitude,
            "homeLon": mapId.homePoint.longitude,
            "alt": alt.value,
            "altType": altType.currentIndex,
            "altCheck": altCheck.checked,
            "missionStep": missionStep.value,
            "horizStep": horizStep.value,
            "horizStepCheck": horizStepCheck.checked,
            "vertStep": vertStep.value,
            "vertStepCheck": vertStepCheck.checked }
        return params
    }

    function setParams(params)
    {
        polygon = params["polygon"]
        drawModePolygon.checked = polygon
        drawModePoints.checked = !polygon
        altValue.value = params["alt"]
        altType.currentIndex = params["altType"]
        checkAlt.checked = params["altCheck"]
        horizStepValue.value = params["horizStep"]
        horizStepCheck.checked = params["horizStepCheck"]
        vertStepValue.value = params["vertStep"]
        vertStepCheck.checked = params["vertStepCheck"]
        missionStepValue.value = params["missionStep"]
    }
}
