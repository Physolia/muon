import QtQuick 2.1
import QtQuick.Controls 1.1
import org.kde.muon 1.0

Item
{
    id: page
    readonly property real proposedMargin: (width-app.actualWidth)/2
    
    function start() {
        updatesModel.prepare()
        updatesModel.updateAll()
    }
    ResourcesUpdatesModel {
        id: updatesModel
        onProgressingChanged: if(!isProgressing) page.Stack.view.pop()
    }
    onVisibleChanged: window.navigationEnabled=!visible
    Binding {
        target: progressBox
        property: "enabled"
        value: !page.visible
    }

    ProgressBar {
        id: progress
        anchors {
            right: parent.right
            left: parent.left
            top: parent.top
            rightMargin: proposedMargin
            leftMargin: proposedMargin
        }
        value: updatesModel.progress
        minimumValue: 0
        maximumValue: 100
        indeterminate: updatesModel.progress==-1
        
        Label {
            anchors.centerIn: parent
            text: updatesModel.remainingTime
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            visible: text!=""
        }
    }

    ScrollView {
        anchors {
            top: progress.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
            rightMargin: proposedMargin
            leftMargin: proposedMargin
            topMargin: 10
            bottomMargin: 10
        }
        ListView {
            id: messageFlickable
            property bool userScrolled: false
            clip: true
            model: updatesModel
            delegate: Label {
                text: display
                height: paintedHeight
                wrapMode: Text.Wrap
                width: messageFlickable.width
            }
            onContentHeightChanged: {
                if(!userScrolled && contentHeight>height && !moving) {
                    contentY = contentHeight - height + anchors.topMargin/2
                }
            }

            //if the user scrolls down, the viewport will be back to following the new progress
            onMovementEnded: userScrolled = !messageFlickable.atYEnd
        }
    }
}
