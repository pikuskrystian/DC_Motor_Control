import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    width: 600
    height: 400

    title: qsTr("Page 2")

    Label {
        text: qsTr("Continous control")
        anchors.verticalCenterOffset: -171
        anchors.horizontalCenterOffset: -215
        anchors.centerIn: parent
    }

    Dial {
        id: dial
        x: 97
        y: 125
        from: 0
    }

    Dial {
        id: dial1
        x: 329
        y: 125
    }

    Label {
        id: label
        x: 141
        y: 94
        text: qsTr("Range of motion")
    }

    Label {
        id: label1
        x: 366
        y: 94
        width: 110
        height: 16
        text: qsTr("Speed of the head")
    }
}
