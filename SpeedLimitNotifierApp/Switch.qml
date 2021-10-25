import QtQuick 2.12

Rectangle {
    id: wSwitch
    property bool switchedOn: false

    height: 25
    width: 50
    radius: height/2

    color: "#5f5f5f"

    states: [
        State {
            when: switchedOn
            PropertyChanges { target: wSwitch; color: "#008d0f" }
            PropertyChanges { target: switchedHandler; x: wSwitch.width - switchedHandler.width; }
        }

    ]

    Rectangle {
        id: switchedHandler
        height: parent.height
        width: height
        radius: parent.radius
        color: "#a5a5a5"
        x: 0
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            wSwitch.switchedOn = !wSwitch.switchedOn;
        }
    }
}
