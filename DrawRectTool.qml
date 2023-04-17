import QtQuick 2.15

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 200
    visible: App.drawToolState === 1
    DrawBtn {
        id: dot
        code: "\ue608"
        isSelected: !App.rectCircle.isFill
        onClick: () => {
                     App.rectCircle.isFill = !App.rectCircle.isFill
                 }
    }
    DrawBtn {
        id: fill
        code: "\ueaa2"
        anchors.left: dot.right
        anchors.top: dot.top
        isSelected: App.rectCircle.isFill
        onClick: () => {
                     App.rectCircle.isFill = !App.rectCircle.isFill
                 }
    }
    Rectangle {
        id: splitter
        height: 20
        width: 1
        color: "#999"
        anchors.verticalCenter: root.verticalCenter
        anchors.left: fill.right
        anchors.leftMargin: 3
    }
    DrawBtn {
        id: emptyRect
        code: "\ue790"
        isSelected: App.rectCircle.isRect
        anchors.left: splitter.right
        anchors.leftMargin: 3
        anchors.top: dot.top
    }
    DrawBtn {
        id: emptyCircle
        code: "\ue610"
        isSelected: !App.rectCircle.isRect
        anchors.left: emptyRect.right
        anchors.top: dot.top
    }
    ColorPicker {
        anchors.left: emptyCircle.right
        anchors.top: dot.top
    }
}
