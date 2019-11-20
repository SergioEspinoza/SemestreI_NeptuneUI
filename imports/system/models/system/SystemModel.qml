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

pragma Singleton
import QtQuick 2.0

import models.can 1.0
import QtApplicationManager 1.0


QtObject {
    id: root

    property bool statusBarExpanded: false
    property bool climateExpanded: false
    property bool windowOverviewVisible: false
    property bool toolBarMonitorVisible: false
    property int currentPageIndex: 2
    property int pageCount: 5
    property bool notificationCenterVisible: false

    //update on direct CanModel signal
//    property Connections canModelConnections : Connections
//    {
//        target: CanModel

        //messageDataUpdate( string bus, int msgId, int data
//        onMessageDataUpdate: {

//            root.notificationCenterVisible = true

//            console.warn( "mesage arrived through signal" )

//            console.warn( "bus : " + bus )
//            console.warn( "msgId : " + msgId )

//            console.warn( "data0 " + data[0].toString(16) )
//            console.warn( "data1 " + data[1].toString(16) )
//            console.warn( "data2 " + data[2].toString(16) )
//            console.warn( "data3 " + data[3].toString(16) )

//        }
//    }


    //update on Notification Manager notificationCenterVisible:

    readonly property Connections notificationManagerConnection: Connections {
        target: NotificationManager

        onNotificationAdded: {
            var receivedContent = NotificationManager.notification(id);
            var body = receivedContent.extended

            var payloadData

            if (receivedContent.category === "can" && body["signalName"] === "unset") {


                console.warn("::: Can message received :::", id);

                payloadData = body["payload"]

                console.warn( " bus : " + body[ "canBus" ] )
                console.warn( "message ID " + body[ "canId" ] )


                console.warn( " can payload [0] " + payloadData[0].toString(16) )
                console.warn( " can payload [1] " + payloadData[1].toString(16) )
                console.warn( " can payload [2] " + payloadData[2].toString(16) )
                console.warn( " can payload [3] " + payloadData[3].toString(16) )

            } else {
                console.warn("::: Can signal changed :::");

                payloadData = body["payload"]

                console.warn( " signal : " + body[ "signalName" ]  )
                console.warn( " signal value : " +  payloadData);
            }
        }
    }


    Component.onCompleted: {

//        CanModel.initCanProcessing( "vcan0", 115200 )

        // Select "AMP" channel (usually "/dev/ttyRPMSG" ).
        CanModel.initCanProcessing("/dev/pts/5", 115200)

    }

}
