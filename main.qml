import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import enhancer.backend 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 600
    title: "Image Enhancer"

    BackEnd {
        id: backend
        algorithm: BackEnd.ShadingCorrectionGaussian
        property Image originalImageElement: Qt.createQmlObject('import QtQuick 2.11; Image{
                    id: originalImage
                    source: "/images/missing.png"
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignTop
                    horizontalAlignment: Image.AlignLeft
                    visible: true
                }',content,"dynamicSnippet_CreateOriginalImage_Dummy");
        property Image enhancedImageElement
        onOriginalImagePathChanged: {
            // destroy and recreate the Image element to force QML to get the Image source again..
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
                    visible: false
                    cache: false
                }',content,"dynamicSnippet_CreateOriginalImage");
            console.log("Object originalImageElement created")
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

    Item{
        id: content
        anchors.fill: parent
    }

    menuBar: MenuBar {
        Menu {
            id: fileMenu
            title: "File"
            MenuItem {
                text: "Choose Image .."
                onTriggered: fileDialog.open()
            }
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
            onClosed: {
                // reset the position (attributes get permanently changed by popup via MouseArea)
                fileMenu.x = 0
                fileMenu.y = 40
            }
        }
        Menu {
            id: viewMenu
            title: "View"
            MenuItem {
                text: "Options"
                onTriggered: optionsDialog.open()
            }
        }
        Menu {
            title: "About"
            MenuItem {
                text: "Info .."
                onTriggered: aboutDialog.open()
            }
        }
    }

    MessageDialog {
        id: aboutDialog
        icon: StandardIcon.Information
        title: "About .."
        text: "Image Enhancer"
        informativeText: "Tool for evaluating image enhancement algorithms. Click the image to show original. See View-Options to switch Algorithm. Created in August 2018 by Thomas Wegmann"
    }

    FileDialog {
        id: fileDialog
        title: "Please choose an image (.jpg/.png)"
        folder: shortcuts.home
        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
        selectMultiple: false
        onAccepted: {
            backend.originalImagePath = fileDialog.fileUrl
            console.log("User chose file: "+fileDialog.fileUrl)
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
                            backend.algorithm = BackEnd.Gaussian
                    }
                }
                RadioButton {
                    text: "Shading Correction (Gauss)"
                    checked: true
                    onCheckedChanged: {
                        if(checked)
                            backend.algorithm = BackEnd.ShadingCorrectionGaussian
                    }
                }
                RadioButton {
                    text: "Shading Correction (Cavalcanti) preview!"
                    onCheckedChanged: {
                        if(checked)
                            backend.algorithm = BackEnd.ShadingCorrectionCavalcanti
                    }
                }
            }
        }
    }

    MouseArea {
        id: leftClickMouseArea
        anchors.fill: content
        acceptedButtons: Qt.LeftButton
        onPressed: {
            if (backend.originalImageElement !== null && backend.enhancedImageElement !== null){
                backend.originalImageElement.visible = true
                backend.enhancedImageElement.visible = false
            }
        }
        onReleased: {
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
        onClicked: fileMenu.popup()
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                fileMenu.popup()
        }
    }

}
