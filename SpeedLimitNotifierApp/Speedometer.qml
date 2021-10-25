import QtQuick 2.12
import QtQuick.Shapes 1.12

Item {
    id: speedometer

    default property alias content: currentSpeedLabelWrapper.children
    property int speedMin: 0
    property int speedMax: 150
    property int speedLimit: 65
    property string speedUnits: qsTr("km/h")
    property double speed: (speedMin+speedMax)/2.0

    property int checkpointStep: 10

    property int thick: 40

    property var hightlited: [60, 90, 110, 150]

    signal checkpointClicked(double speed);
    signal limitClicked(double limit);
    signal speedLimitChangeComplete(double newLimit);

    onCheckpointClicked: {
        speedometer.speedLimit = speed;
        speedometer.speedLimitChangeComplete(speedometer.speedLimit);
    }


    QtObject {
        id: _private
        property double speedDiff: speedometer.speedMax - speedometer.speedMin
        property int radius: Math.min(width/2.0, height)
        property double sanSpeed: Math.min(Math.max(speedometer.speed, speedometer.speedMin), speedometer.speedMax);

        function speedAngle(speed)
        {
            return (speed / _private.speedDiff -  speedometer.speedMin/_private.speedDiff ) * 180.0;
        }
    }

    Shape {
        id: progress
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 4
        property double centerX: width/2.0
        property double centerY: height/2.0 + _private.radius/2.0

        ShapePath {
            strokeWidth: 3
            strokeColor: "#3f3f3f"

            fillGradient: ConicalGradient {
                centerX: progress.centerX
                centerY: progress.centerY
                angle: 180.0 - _private.speedAngle(_private.sanSpeed)
                GradientStop { position: 0.0; color: "#3575d5" }
                GradientStop { position: 1; color: "#2e2e2e" }
            }

            PathAngleArc {
                centerX: progress.centerX
                centerY: progress.centerY
                radiusX: _private.radius
                radiusY: _private.radius
                startAngle: 180
                sweepAngle: 180
            }

            PathAngleArc {
                centerX: progress.centerX
                centerY: progress.centerY
                radiusX: _private.radius - speedometer.thick
                radiusY: _private.radius - speedometer.thick
                startAngle: 180
                sweepAngle: 180
            }
        }
    }


    Repeater {
        id: checkpointLables
        property int count: _private.speedDiff / speedometer.checkpointStep +1
        property int lift: 10
        model: count
        property double angleStep: 180.0 / (count-1);
        delegate: Item {
            id: checkpointLabelWrapper
            property int speed: (speedometer.speedMin + model.index*speedometer.checkpointStep).toFixed(0)
            property bool isHightlited: (speedometer.hightlited.indexOf(speed)>=0)
            x: (_private.radius-speedometer.thick/2) * Math.cos((180.0-model.index*checkpointLables.angleStep)*Math.PI/180.0) + progress.centerX - width/2.0
            y: (_private.radius-checkpointLables.lift-speedometer.thick/2) * Math.sin(-(model.index*checkpointLables.angleStep)*Math.PI/180.0) + progress.centerY - checkpointLables.lift - height/2.0
            width: 20
            height: 20

            Text {
                anchors.centerIn: parent
                text:checkpointLabelWrapper.speed
                color: "#fff"
                font.bold: (checkpointLabelWrapper.isHightlited || checkpointLabelMouseArea.containsMouse)
            }

            MouseArea {
                id: checkpointLabelMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    speedometer.checkpointClicked(checkpointLabelWrapper.speed);
                }
            }
        }
    }

    Repeater {
        id: checkpointLines
        property int length: _private.radius/10.0
        property int count: _private.speedDiff / speedometer.checkpointStep +1
        property int lift: 14
        property int offset: _private.radius/25.0
        model: count
        property double angleStep: 180.0 / (count-1);
        delegate: Rectangle {
            id: checkpointLineWrapper
            x: (_private.radius-speedometer.thick - checkpointLines.length/2 - checkpointLines.offset) *  Math.cos((180.0-model.index*checkpointLables.angleStep)*Math.PI/180.0) + progress.centerX
            y: (_private.radius-checkpointLines.lift - speedometer.thick - checkpointLines.offset) *  Math.sin(-(model.index*checkpointLables.angleStep)*Math.PI/180.0) + progress.centerY - checkpointLines.lift
            color: "#353535"
            width: 2
            height: checkpointLines.length
            rotation: model.index*checkpointLines.angleStep+90.0
            antialiasing: true
        }
    }

    Item {
        id: currentSpeedLabel
        width: _private.radius*2.0
        x: progress.centerX - width/2
        y: progress.centerY - childrenRect.height

        Column {
            id: currentSpeedLabelWrapper
            spacing: 0
            width: parent.width

            Text {
                id: speedValue
                width: parent.width
                height: contentHeight
                text: _private.sanSpeed.toFixed(0)
                font.pixelSize: _private.radius/3.0
                horizontalAlignment: Text.AlignHCenter
                color: "#3070ce"
            }

            Text {
                id: speedUnits
                width: parent.width
                height: contentHeight
                text: speedometer.speedUnits
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: _private.radius/10.0
                color: "#5f5f5f"
            }
        }
    }


    Item {
        id: limiterWrapper
        property double limit: speedometer.speedLimit


        visible: (limit>0)
        width: _private.radius/6
        height: width
        x: (_private.radius-speedometer.thick/2.0) *  Math.cos((180.0 - _private.speedAngle(limiterWrapper.limit)) * Math.PI/180.0) + progress.centerX - width/2
        y: (_private.radius-speedometer.thick/2.0) *  Math.sin(-_private.speedAngle(limiterWrapper.limit) * Math.PI/180.0) + progress.centerY - height/2
        Shape {
            width: limiterWrapper.width
            height: limiterWrapper.height
            antialiasing: true
            layer.enabled: true
            layer.samples: 4

            ShapePath {
                strokeWidth: 4
                strokeColor: "#bb0000"
                PathAngleArc {
                    centerX: limiterWrapper.width/2
                    centerY: limiterWrapper.height/2
                    radiusX: limiterWrapper.width/2-2
                    radiusY: limiterWrapper.height/2-2
                    startAngle: 0
                    sweepAngle: 360
                }
                fillColor: "#fff"
            }
        }

        Text {
            id: limitValue
            anchors.centerIn: parent
            text: limiterWrapper.limit.toFixed(0)
            color: "#000"
            font.bold: true
            font.pixelSize: _private.radius/14
        }

        MouseArea {
            anchors.fill: parent
            property bool dragged: false
            onPressed: {
                dragged = true;
            }
            onReleased: {                
                speedometer.speedLimitChangeComplete(limiterWrapper.limit);
                dragged = false;                
            }

            onPositionChanged: {
                if ( !dragged ) { return; }
                var p = speedometer.mapFromItem(limiterWrapper, mouseX, mouseY);
                var angle = Math.PI + Math.atan2(p.y-progress.centerY, p.x-progress.centerX);
                angle = (angle>Math.PI)? ((angle>Math.PI*1.5)? 0 : Math.PI) : angle;
                speedometer.speedLimit = (angle/Math.PI )* _private.speedDiff + speedometer.speedMin;
            }
        }
    }
}
