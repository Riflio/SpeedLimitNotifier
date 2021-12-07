import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("SpeedLimitNotifier")

    Material.theme: Material.Dark
    Material.accent: Material.DeepOrange

    header: RowLayout {
        id: headerWrapper
        width: parent.width
        property var curPageItem: pages.itemAt(pages.currentIndex);

        Button {
            id: showSideBar
            Layout.alignment: Qt.AlignLeft
            implicitWidth: 40
            display: Button.IconOnly
            flat: true
            icon.source: "qrc:/Assets/Menu.svg"
            icon.height: 15
            icon.width: 15
            onClicked:  {
                sideBar.visible = !sideBar.visible;
            }
        }

        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            text: ((headerWrapper.curPageItem.hasOwnProperty("titlePrefix"))? headerWrapper.curPageItem.titlePrefix:"")+headerWrapper.curPageItem.title
            font.capitalization: Font.AllUppercase
            font.bold: true
        }

        Loader {
            id: titleContentLoader
            visible: (!!sourceComponent && headerWrapper.curPageItem.enabled)
            sourceComponent: (headerWrapper.curPageItem.hasOwnProperty("titleComponent"))? headerWrapper.curPageItem.titleComponent : null
        }

        Item {
            visible: !titleContentLoader.visible
            Layout.preferredWidth: showSideBar.width
        }
    }


    Drawer {
        id: sideBar
        width: Math.min(0.66*parent.width, 300)
        height: parent.height

        ColumnLayout {
            id: sideBarContent
            anchors.fill: parent
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: logoImgText.height

                Image {
                    id: logoImgText
                    width: parent.width
                    source: "qrc:/Assets/Logo-text.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    antialiasing: true
                }
            }

            Flickable {
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: width
                contentHeight: mainMenuItemsWrapper.implicitHeight
                clip: true

                ColumnLayout {
                    id: mainMenuItemsWrapper
                    width: parent.width

                    Repeater {
                        id: mainMenuItems
                        model: pages.count
                        delegate: ItemDelegate {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            text: pages.itemAt(model.index).title
                            enabled: (pages.currentIndex!==model.index && pages.itemAt(model.index).enabled)
                            font.bold: (pages.currentIndex===model.index)
                            onClicked: {
                                pages.currentIndex = model.index;
                                sideBar.close();
                            }
                        }
                    }

                    Image {
                        Layout.fillWidth: true
                        source: "qrc:/Assets/AppIcon.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        antialiasing: true
                        opacity: 0.4
                    }
                }
            }
        }
    }


    StackLayout {
        id: pages
        anchors.fill: parent
        currentIndex: 0

        Page {
            id: pageSpeed
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Speed control")

            property Component titleComponent: RowLayout {
                Label {
                    text: qsTr("Enabled: ")
                }

                Switch {
                    id: enableSwitch
                    position: (appCore.enabled)? 1.0 : 0.0
                    readonly property bool activated: (enableSwitch.visualPosition!==0.0)
                    onPositionChanged: {
                        appCore.enabled = enableSwitch.visualPosition;
                    }

                    Connections { //-- На случай, если нельзя включить или состояние изменилось
                        target: appCore
                        onEnabledChanged: {
                            if ( appCore.enabled!==enableSwitch.activated ) {
                                enableSwitch.position = appCore.enabled;
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                anchors.fill: parent

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

                    ColumnLayout {
                        anchors.horizontalCenter: parent.horizontalCenter                        
                        spacing: 10

                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                            Image {
                                source: "qrc:/Assets/SatellitesIcon.png"
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                            }

                            Text {
                                text: ((appCore.satellitesCount<=0)? qsTr("Not found"): appCore.satellitesCount)
                                font.pixelSize: 14
                                color: "#5f5f5f"
                            }
                        }

                        Text {
                            id: lastErrorText
                            visible: (appCore.lastErrorText!=="")
                            text: appCore.lastErrorText
                            color: "red"
                        }
                    }
                }                
            }
        } //-- End page Speed

        Page {
            id: pageSettings
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Settings")

            ColumnLayout {
                anchors.fill: parent

                Item {
                    Layout.fillHeight: true
                }

                Label {
                    text: qsTr("Notify signal type:")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                }

                ComboBox {
                    id: notifySignalType
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    model: [qsTr("None"), qsTr("Tone"), qsTr("File")]
                    currentIndex: appCore.notifySignalType
                    onCurrentIndexChanged: {
                        appCore.notifySignalType = notifySignalType.currentIndex;
                    }
                }

                Button {
                    text: qsTr("Sound test")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    onClicked: {
                        appCore.notifySignalTest();
                    }
                }

                Label {
                    text: qsTr("Autoload:")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                }

                Switch {
                    id: autoloadSwitch
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    enabled: false
                    position: (appCore.autoload)? 1.0 : 0.0
                    onPositionChanged: {
                        appCore.autoload = autoloadSwitch.visualPosition;
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        } //-- End page Settings

        Page {
            id: pageAbout
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("About")

        } //-- End page About
    }
}
