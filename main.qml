import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import enhancer.backend 1.0
import enhancer.enums 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 600
    title: "Image Enhancer"

    BackEnd {
        id: backend
        algorithm: ImageEnhancerEnums.ShadingCorrectionGaussian
        property Image originalImageElement:
            Qt.createQmlObject('import QtQuick 2.11; Image{
                    id: originalImage
                    source: "/images/missing.png"
                    anchors.fill: parent
                    verticalAlignment: Image.AlignTop
                    horizontalAlignment: Image.AlignLeft
                    visible: true
                    property bool isInitialImage: true
                }',content,"dynamicSnippet_CreateOriginalImage_Dummy");
        property Image enhancedImageElement
        onOriginalImagePathChanged: {
            console.log("ORIGINAL image changed")
            if (originalImageElement !== null)
                originalImageElement.destroy()
            console.log("originalImageElement.destroy()")
            backend.originalImageElement = Qt.createQmlObject('import QtQuick 2.11; Image{
                    id: originalImage
                    source: backend.originalImagePath
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignTop
                    horizontalAlignment: Image.AlignLeft
                    visible: true
                    cache: false
                    property bool isInitialImage: false
                }',content,"dynamicSnippet_CreateOriginalImage");
            console.log("Object originalImageElement created")
            console.log("ALGORITHM started")
        }
        onEnhancedImagePathChanged: {
            console.log("ENHANCED image changed")
            if (enhancedImageElement !== null)
                enhancedImageElement.destroy()
            console.log("enhancedImageElement.destroy()")
            backend.enhancedImageElement = Qt.createQmlObject('import QtQuick 2.11; Image{
                    id: enhancedImage
                    source: backend.enhancedImagePath
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignTop
                    horizontalAlignment: Image.AlignLeft
                    visible: true
                    cache: false // necessary because source path stays the same
                }',content,"dynamicSnippet_CreateEnhancedImage");
            console.log("Object enhancedImageElement created")
        }
    }

    menuBar: MenuBar {
        Menu {
            id: fileMenu
            title: "File"
            MenuItem {
                text: "Open..."
                onTriggered: fileDialog.open()
                Shortcut {
                    sequence: StandardKey.Open
                    onActivated: fileDialog.open()
                }
            }
            MenuItem {
                text: "Options..."
                onTriggered: optionsDialog.open()
            }
            MenuSeparator{}
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
                Shortcut {
                    sequence: StandardKey.Quit
                    onActivated: Qt.quit()
                }
            }
        }
        Menu {
            title: "About"
            MenuItem {
                text: "Info..."
                onTriggered: aboutDialog.open()
            }
        }
    }

    Item{
        id: content
        anchors.fill: parent
    }

    footer: GroupBox {
        Layout.fillWidth: true
        BusyIndicator {
            id: busyIndicator
            running: backend.isProcessing
        }
    }

    MessageDialog {
        id: aboutDialog
        icon: StandardIcon.Information
        title: "About .."
        text: "Image Enhancer"
        informativeText: "Tool for evaluating image enhancement algorithms. Click the image to show original. See Options to change and configure algorithm."
        detailedText: "Created in August 2018 by Thomas Wegmann"
    }

    FileDialog {
        id: fileDialog
        title: "Please choose an image (.jpg/.png)"
        folder: shortcuts.home
        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
        selectMultiple: false
        onAccepted: {
            console.log("User chose file: "+fileDialog.fileUrl)
            backend.originalImagePath = fileDialog.fileUrl
        }
    }

    Dialog{
        id: optionsDialog
        GroupBox {
            title: "Algorithm to apply"
            RowLayout {
                RadioButton {
                    text: "Gaussian"
                    onCheckedChanged: {
                        if(checked)
                            backend.algorithm = ImageEnhancerEnums.Gaussian
                    }
                }
                RadioButton {
                    text: "Shading Correction (Gauss)"
                    checked: true
                    onCheckedChanged: {
                        if(checked)
                            backend.algorithm = ImageEnhancerEnums.ShadingCorrectionGaussian
                    }
                }
                RadioButton {
                    text: "Shading Correction (Cavalcanti) preview!"
                    onCheckedChanged: {
                        if(checked)
                            backend.algorithm = ImageEnhancerEnums.ShadingCorrectionCavalcanti
                    }
                }
            }
        }
    }

    MouseArea {
        id: leftClickMouseArea
        anchors.fill: content
        acceptedButtons: Qt.LeftButton
        onPressed: { // display original image or open file dialog if no image was loaded yet
            if (backend.originalImageElement !== null){
                if (backend.enhancedImageElement !== null){
                    backend.originalImageElement.visible = true
                    backend.enhancedImageElement.visible = false
                } else {
                    if (backend.originalImageElement.isInitialImage)
                        fileDialog.open()
                }
            }
        }
        onReleased: { // display enhanced image
            if (backend.originalImageElement !== null)
                backend.originalImageElement.visible = true
            if (backend.originalImageElement !== null && backend.enhancedImageElement !== null){
                backend.originalImageElement.visible = false
                backend.enhancedImageElement.visible = true
            }
        }
    }

    MouseArea {
        id: rightClickMouseArea
        anchors.fill: content
        acceptedButtons: Qt.RightButton
        onClicked: optionsDialog.open()
        onPressAndHold: { // so the dialog doesnt open when drag-drop motion was made
            if (mouse.source === Qt.MouseEventNotSynthesized)
                optionsDialog.open()
        }
    }

}
