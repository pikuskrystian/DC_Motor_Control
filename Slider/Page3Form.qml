import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    width: 600
    height: 400

    title: qsTr("Step work")

    Label {
        text: qsTr("Step Work.")
        anchors.centerIn: parent
    }
    Dial {
        id: dial3
        x: 97
        y: 125
        from: 0

    }
}
