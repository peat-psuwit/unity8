AbstractButton {
                id: root;
                property var components;
                property var cardData;
                property var artShapeBorderSource: undefined;
                property real fontScale: 1.0;
                property var scopeStyle: null;
                property int titleAlignment: Text.AlignLeft;
                property int fixedHeaderHeight: -1;
                property size fixedArtShapeSize: Qt.size(-1, -1);
                readonly property string title: cardData && cardData["title"] || "";
                property bool asynchronous: true;
                property bool showHeader: true;
                implicitWidth: childrenRect.width;
                enabled: true;

readonly property size artShapeSize: Qt.size(-1, -1);
readonly property int headerHeight: titleLabel.height + subtitleLabel.height + subtitleLabel.anchors.topMargin + audioProgressBar.height + audioProgressBar.anchors.topMargin;
Label {
                        id: titleLabel;
                        objectName: "titleLabel";
                        anchors { right: parent.right;
rightMargin: units.gu(1);
left: audioButton.right;
                            leftMargin: units.gu(1);
top: parent.top;
                                     topMargin: units.gu(1);
 }
                        elide: Text.ElideRight;
                        fontSize: "small";
                        wrapMode: Text.Wrap;
                        maximumLineCount: 2;
                        font.pixelSize: Math.round(FontUtils.sizeToPixels(fontSize) * fontScale);
                        color: root.scopeStyle ? root.scopeStyle.foreground : Theme.palette.normal.baseText;
                        visible: showHeader ;
                        text: root.title;
                        font.weight: cardData && cardData["subtitle"] ? Font.DemiBold : Font.Normal;
                        horizontalAlignment: root.titleAlignment;
                    }
Label {
                            id: subtitleLabel;
                            objectName: "subtitleLabel";
                            anchors { left: titleLabel.left;
                               leftMargin: titleLabel.leftMargin;
rightMargin: units.gu(1);
right: titleLabel.right;
top: titleLabel.bottom;
 }
                            anchors.topMargin: units.dp(2);
                            elide: Text.ElideRight;
                            maximumLineCount: 1;
                            fontSize: "x-small";
                            font.pixelSize: Math.round(FontUtils.sizeToPixels(fontSize) * fontScale);
                            color: root.scopeStyle ? root.scopeStyle.foreground : Theme.palette.normal.baseText;
                            visible: titleLabel.visible && titleLabel.text;
                            text: cardData && cardData["subtitle"] || "";
                            font.weight: Font.Light;
                        }
CardAudioProgress {
                            id: audioProgressBar;
                            duration: (cardData["quickPreviewData"] && cardData["quickPreviewData"]["duration"]) || 0;
                            source: (cardData["quickPreviewData"] && cardData["quickPreviewData"]["uri"]) || "";
                            anchors {
                                bottom: audioButton.bottom;
                                left: audioButton.right;
                                right: parent.right;
                                margins: units.gu(1);
                            }
                            color: root.scopeStyle ? root.scopeStyle.foreground : Theme.palette.normal.baseText;
                         }AbstractButton {
                            id: audioButton;
                            anchors.fill: undefined;
                            width: height;
                            height: (root.fixedHeaderHeight > 0 ? root.fixedHeaderHeight : headerHeight) + 2 * units.gu(1);
                            readonly property url source: (cardData["quickPreviewData"] && cardData["quickPreviewData"]["uri"]) || "";
                            readonly property url playlist: (cardData["quickPreviewData"] && cardData["quickPreviewData"]["playlist"]) || null;
                            UbuntuShape {
                                anchors.fill: parent;
                                visible: parent.pressed;
                                radius: "medium";
                                borderSource: "radius_pressed.sci"
                            }
                            Icon {
                                anchors.fill: parent;
                                anchors.margins: parent.height > units.gu(5) ? units.gu(2) : units.gu(0);
                                opacity: 0.9;
                                name: DashAudioPlayer.isCurrentSource(parent.source) && DashAudioPlayer.playing ? "media-playback-pause" : "media-playback-start";
                            }
                            onClicked: {
                                if (DashAudioPlayer.isCurrentSource(source)) {
                                    if (DashAudioPlayer.playing) {
                                        DashAudioPlayer.pause();
                                    } else {
                                        DashAudioPlayer.play();
                                    }
                                } else {
                                    DashAudioPlayer.playSource(source, playlist);
                                }
                            }
                            onPressAndHold: {
                                root.pressAndHold();
                            }
                        }UbuntuShape {
                        id: touchdown;
                        objectName: "touchdown";
                        anchors { fill: root }
                        visible: root.pressed;
                        radius: "medium";
                        borderSource: "radius_pressed.sci"
                    }
implicitHeight: audioButton.height;
}
