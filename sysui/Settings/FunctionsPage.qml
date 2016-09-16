/****************************************************************************
**
** Copyright (C) 2016 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Neptune IVI UI.
**
** $QT_BEGIN_LICENSE:GPL-QTAS$
** Commercial License Usage
** Licensees holding valid commercial Qt Automotive Suite licenses may use
** this file in accordance with the commercial license agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and The Qt Company.  For
** licensing terms and conditions see https://www.qt.io/terms-conditions.
** For further information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
** SPDX-License-Identifier: GPL-3.0
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import controls 1.0
import utils 1.0
import service.settings 1.0
import service.notification 1.0

UIPage {
    id: root
    hspan: 24
    vspan: 24

    title: qsTr('Car Settings')
    symbolName: "settings"

    NotificationInterface {
        id: notificationInterface
        actions: ["OK"]
        summary: "Car Settings"
    }

    GridView {
        id: view

        anchors.top: parent.top
        anchors.topMargin: Style.vspan(2)
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        interactive: false

        width: Style.hspan(6*3)

        model: SettingsService.functions

        clip: false // true

        cellWidth: Style.hspan(6)
        cellHeight: Style.vspan(5)

        delegate: Item {
            property bool active: model.active

            width: view.cellWidth
            height: view.cellHeight

            HDiv {
                id: div
                width: parent.width
                vspan: 1
            }

            RowLayout {
                spacing: Style.paddingXL
                anchors.top: div.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 4

                Item {
                    Layout.preferredWidth: rect.width
                    Layout.preferredHeight: parent.height

                    RectangularGlow {
                        anchors.fill: rect
                        glowRadius: 5
                        spread: 0
                        color: Style.colorOrange
                        cornerRadius: 10
                        visible: active
                    }

                    Rectangle {
                        id: rect
                        color: active ? Style.colorOrange : "#444"
                        width: 4
                        radius: 2
                        height: parent.height
                    }
                }

                Image {
                    source: Style.symbolM(model.icon)
                }

                Label {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: model.description
                    wrapMode: Text.WordWrap
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    active = !active
                    notificationInterface.body = model.description + (active ? " activated" : " deactivated")
                    notificationInterface.show()
                }
            }
        }
    }
}
