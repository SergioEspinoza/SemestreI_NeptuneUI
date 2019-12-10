/****************************************************************************
**
** Copyright (C) 2017 Pelagicore AG
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

import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import utils 1.0
import controls 1.0
import models.statusbar 1.0
import models.settings 1.0
import models.system 1.0

Pane {
    id: root
    objectName: "StatusBar"
    anchors.top: parent.top
    width: Style.hspan(24)
    height: Style.statusBarHeight

    MouseArea {
        objectName: "StatusBar::MouseArea"
        anchors.fill: parent
        onClicked: {
            if (!SettingsModel.settingsPageVisible) {
                if (SystemModel.climateExpanded) {
                    SystemModel.climateExpanded.expanded = false
                } else {
                    SystemModel.statusBarExpanded = true
                }
            }
        }
        onPressAndHold: SystemModel.toolBarMonitorVisible = !SystemModel.toolBarMonitorVisible
    }

    IndicatorTray {
        model: StatusBarModel.indicators
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: Style.padding
        anchors.bottomMargin: Style.padding
    }

    PageIndicator {
        objectName: "StatusBar::PageIndicator"
        anchors.centerIn: parent

        currentIndex: SystemModel.currentPageIndex
        count: SystemModel.pageCount
    }

    RowLayout {
        spacing: Style.paddingXL * 2 // 1920 x 1080
//        spacing: Style.paddingXL * 4 // 1280 x 800
        Layout.maximumWidth: Style.hspan(12)
        Layout.maximumHeight: Style.statusBarHeight
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: Style.padding
        anchors.bottomMargin: Style.padding

        Weather {
            visible: !Style.isPotrait
        }

        DateAndTime {
            timeFormat: SettingsModel.clockOption.format
            currentDate: StatusBarModel.currentDate
        }

        Tool {
            objectName: "StatusBar::FullScreenLabel"
            width: Style.hspan(0.85)
            height: Style.vspan(1.05)
            symbol: "fullscreen"
            onClicked: SystemModel.windowOverviewVisible = true
        }

        Tool {
            objectName: "StatusBar::SliderLabel"
            width: Style.hspan(0.85)
            height: Style.vspan(1.05)
            symbol: "slider_marker" // TODO: Replace with notification icon when available
            onClicked: SystemModel.notificationCenterVisible = !SystemModel.notificationCenterVisible
        }
    }
}
