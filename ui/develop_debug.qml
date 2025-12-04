import QtQuick
import QtQuick.Controls

Window {
    width: 800; height: 600
    visible: true
    title: "TheCalculater"

    Text {
        x: 70; y: 40
        width: 181; height: 31
        font.pointSize: 18
        text: "Develop Debug UI"
    }

    Button {
        x: 70; y: 120;
        width: 221; height: 41
        text: "Click me to log Hello World"
        onClicked: {
            console.log("Hello World");
        }
        font.pointSize: 11
    }
}