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
import animations 1.0
import QtApplicationManager 1.0

import com.yazaki.CanController 1.0
import service.can 1.0

import utils 1.0


import QtQuick.Layouts 1.0

import controls 1.0
import models.settings 1.0
import service.popup 1.0
import service.notification 1.0



QtObject {
    id: root

    //WARNING: optimal way for listening to changing CAN signal / message values is:
    //  Low frequency event:
    //       Set up notificationManager "onNotificationAdded" (like below), and wait
    //       for notification to be sent via system wide notification. Then retrieve
    //       and read notification object
    //  High frequency event:
    //       Setup Javascript callback directly into CanController plugin singlelton\
    //       via setCanMsgNotificationCallback / setCanSignalNotificationCallback
    //  (not yet supported)
    //  Third alternative:
    //        In the case that system wide notifications are not available,
    //        and can bus carries low traffic, and event is triggered on
    //        low frequency basis, direct slot to "signalValueUpdate" / "messageDataUpdate"
    //        can be implemented.


    function initCanProcessing( devname, bitrate ){
        //change based on available buses

        CanController.canInit( devname, bitrate )
        CanController.initCanRx( devname )
    }

    signal signalValueUpdate(  string signalName, int value )

    // data corresponds to 64 bit value
    signal messageDataUpdate( string bus, int msgId, var data )



//    function setCanMsgNotificationCallback( busName, msgId, callback )
//    {
//            //TODO: implement in CanController backend
//            //canController.setCanMsgNotificationCallback

//    }

//    function setCanSignalNotificationCallback( busName, signalName, callback )
//    {
//        //TODO: implement in CanController backend
//        //canController.setCanSignalNotificationCallback

//    }


   property CanNotificationInterface canInterface :  CanNotificationInterface{
        //set
        id: canNotification
    }

    property Connections canConnections : Connections {
        target: CanController

        // \brief OnRxMessageDataChanged
        //        incoming CAN  message data changes
        //  \param devName 'string' , associated CAN bus
        //  \param frame 'QVariantMap' type with following memebers:
        //               'frameId' : 32 bit integer
        //               'frameType' : string
        //               'payload' : message payload

        onRxMessageDataChanged: {
            //var busName = devName
            var msgId = frame[ "frameId" ]
            var frameType = frame["frameType"]
            var payload = new Int8Array(frame[ "payload" ])

            //unable to propagate directly as Int8Array object
            var payload2 = [ payload[0], payload[1], payload[2], payload[3] ]

            canNotification.canBus = devName
            canNotification.canId = msgId
            canNotification.payload = payload2


            //send via notification manager
            canNotification.show()


            //send via signal / slot mechanism
            root.messageDataUpdate( devName, msgId, payload )

        }

        /*!
        * \brief rxSignalValueChanged
        * incoming CAN Signal  changes
        *  \param signalName 'string' type, unique signal name
        *  \param value 'int' type, signal value
        */
        onRxSignalValueChanged: {
            //TODO: implement!
            var name = signalName
            var signalValueArr = [value, value, value, value]
            var signalValue = value

            canNotification.signalName = name
            canNotification.payload[0] = signalValue
            canNotification.show()

            root.signalValueUpdate(name, signalValue)

            if (value > 7) {
                SettingsModel.functions.setProperty(signalValue - 8, "active", true)
                canNotification.body = name + " activated"
                canNotification.show()
            } else {
                SettingsModel.functions.setProperty(signalValue, "active", false)
                canNotification.body = name + " deactivated"
                canNotification.show()
            }
        }

    }


  property Connections notifyConnections : Connections {
      target: NotificationManager

      // process incoming notifications for self originated signals
      onNotificationAdded: {

          //TODO: check associated signal / value and emit either
          // signalValueChanged or messgeDataChanged signals

          var receivedContent = NotificationManager.notification(id);
          //TODO: process notification and update message value with
          // CanController backend

          }
      }

    Component.onCompleted: {

    }


}
