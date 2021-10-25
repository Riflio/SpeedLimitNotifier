import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Rectangle {
        id: mainWrapper
        anchors.fill: parent
        color: "#424242"

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                Layout.rightMargin: 10
                Layout.leftMargin: 10
                Item {
                    Layout.fillWidth: true
                }

                Text {
                    text: qsTr("Enabled: ")
                }

                Switch {
                    id: enableSwitch
                    switchedOn: appCore.enabled
                    onSwitchedOnChanged: {
                        appCore.enabled = switchedOn;
                    }

                    Connections { //-- На случай, если нельзя включить или состояние изменилось
                        target: appCore
                        onEnabledChanged: {
                            if ( appCore.enabled!==enableSwitch.switchedOn ) {
                                enableSwitch.switchedOn = appCore.enabled;
                            }
                        }
                    }

                }
            }

            Speedometer {
                id: speedometer
                Layout.fillWidth: true
                Layout.fillHeight: true
                speedMin: 0
                speedMax: 150
                speedLimit: appCore.limit
                speed: appCore.speed

                onSpeedLimitChangeComplete: {
                    appCore.limit = newLimit;
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 16
                    spacing: 10

                    Image {
                        source: "qrc:/Assets/SatellitesIcon"
                        width: 16
                        height: width
                    }

                    Text {
                        text: ((appCore.satellitesCount<=0)? qsTr("Not found"): appCore.satellitesCount)
                        font.pixelSize: 14
                        color: "#5f5f5f"
                    }

                }
            }
        }

    }
}
