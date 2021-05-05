import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    width: 600
    height: 400

    title: qsTr("Page 1")

    Label {
        text: qsTr("Manual control of the head position")
        anchors.verticalCenterOffset: -180
        anchors.horizontalCenterOffset: -163
        anchors.centerIn: parent
    }

    Slider {
        id: slider1
        x: 114
        y: 74
        width: 373
        height: 40
        value: 0.5
    }

    Switch {
        id: switch1
        x: 249
        y: 0
    }
}
