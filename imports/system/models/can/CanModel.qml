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
import QtQuick 2.6
import animations 1.0
import QtApplicationManager 1.0

import com.yazaki.CanController 1.0
import service.can 1.0




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

    signal signalValueUpdate(  string signalName, int value )

    // data corresponds to 64 bit value
    signal messageDataUpdate( string bus, int msgId, int data )



    function setCanMsgNotificationCallback( busName, msgId, callback )
    {
            //TODO: implement in CanController backend
            //canController.setCanMsgNotificationCallback

    }

    function setCanSignalNotificationCallback( busName, signalName, callback )
    {
        //TODO: implement in CanController backend
        //canController.setCanSignalNotificationCallback

    }


   CanNotificationInterface  {
        //set
        id: canNotification
    }


    CanController  {

        id: canController

        // \brief OnRxMessageDataChanged
        //        incoming CAN  message data changes
        //  \param devName 'string' , associated CAN bus
        //  \param frame 'QVariantMap' type with following memebers:
        //               'frameId' : 32 bit integer
        //               'frameType' : string
        //               'payload' : message payload

        onRxMessageDataChanged: {
            var busName = devName
            var msgId = frame[ "msgId" ]
            var payload = frame[ "payload" ]

            canNotification.canBus = "CCAN"
            canNotification.canId = msgId
            canNotification.payload = payload

            //send via notification manager
            canNotification.show()


            //send via signal / slot mechanism
            emit( messageDataUpdate( busName, msgId, payload ) )


        }

        /*!
        * \brief rxSignalValueChanged
        * incoming CAN Signal  changes
        *  \param signalName 'string' type, unique signal name
        *  \param value 'int' type, signal value
        */
       onRxSignalValueChanged: {
            //TODO: implement!

        }

    }




    Connections {
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


}
