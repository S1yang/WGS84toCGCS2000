import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id: window
    width: 400
    height: 500
    visible: true
    title: qsTr("WGS84 -> CGCS2000 Converter")

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        padding: 20

        // 纬度输入
        Column {
            spacing: 2
            Text { text: "纬度 (度):" }
            TextField {
                id: latField
                placeholderText: "例如：39.9042"
                text: "39.9042"
            }
        }

        // 经度输入
        Column {
            spacing: 2
            Text { text: "经度 (度):" }
            TextField {
                id: lonField
                placeholderText: "例如：116.4074"
                text: "116.4074"
            }
        }

        // 中央子午线输入
        Column {
            spacing: 2
            Text { text: "中央子午线 (度):" }
            TextField {
                id: cmField
                placeholderText: "例如：117"
                text: "117"
            }
        }

        // 单点转换按钮
        Button {
            text: "单点转换"
            onClicked: {
                var lat = parseFloat(latField.text)
                var lon = parseFloat(lonField.text)
                var cm = parseFloat(cmField.text)
                var result = converter.convert(lat, lon, cm)
                resultDialog.resultText = "X: " + result.x + "\nY: " + result.y
                resultDialog.open()
            }
        }

        // 批量转换按钮
        Button {
            text: "批量转换"
            onClicked: {
                // 打开文件选择对话框选择包含多组坐标的TXT文件
                batchFileDialog.open()
            }
        }
    }

    // 转换结果显示对话框
    Dialog {
        id: resultDialog
        title: "转换结果"
        // 去掉标准按钮
        // standardButtons: Dialog.Ok

        property string resultText: ""

        contentItem: Column {
            spacing: 10
            padding: 10

            TextArea {
                id: resultTextArea
                text: resultDialog.resultText
                readOnly: true
                wrapMode: TextArea.Wrap
                selectByMouse: true

            }

            Row {
                spacing: 10

                Button {
                    text: "复制"
                    onClicked: {
                        converter.copyToClipboard(resultTextArea.text)
                    }
                }
                Button {
                    text: "保存为TXT"
                    onClicked: {
                        saveFileDialog.open()
                    }
                }
                Button {
                    text: "关闭"
                    onClicked: resultDialog.close()
                }
            }
        }
    }


    // 用于单点转换保存结果的文件对话框
    FileDialog {
        id: saveFileDialog
        title: "选择保存路径"
        nameFilters: ["Text files (*.txt)"]
        onAccepted: {
            var filePath = fileDialogToPath(fileUrl)
            if (converter.saveTextToFile(resultDialog.resultText, filePath)) {
                console.log("保存成功：" + filePath)
            } else {
                console.log("保存失败")
            }
        }
        onRejected: {
            console.log("取消保存")
        }
    }

    // 用于批量转换的文件选择对话框
    FileDialog {
        id: batchFileDialog
        title: "选择批量转换的TXT文件"
        nameFilters: ["Text files (*.txt)"]
        selectExisting: true
        onAccepted: {
            var inputFilePath = fileDialogToPath(fileUrl)
            var cm = parseFloat(cmField.text)
            var batchResult = converter.batchConvertToString(inputFilePath, cm)
            resultDialog.resultText = batchResult
            resultDialog.open()
        }
        onRejected: {
            console.log("取消选择批量转换文件")
        }
    }

    // 辅助函数：将 fileUrl 转为本地路径（去掉 file:/// 前缀）
    function fileDialogToPath(url) {
        var str = url.toString();
        if (str.indexOf("file:///") === 0)
            return str.substring(8);
        return str;
    }
}

