/****************************************************************************
** YAZAKI COPYRIGHT
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


   property CanNotificationInterface canInterface :  {
        //set
        id: canNotification
    }


    property CanController controller :  {

        id: canController

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
            var payload = frame[ "payload" ]

            canNotification.canBus = devName
            canNotification.canId = msgId
            canNotification.payload = payload


            console.warn( "CAN frame id " + msgId )

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




    property Connections connections : {
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

        canController.initCanRx( "vcan0" )

        console.warn( "CanModel Component::OnCompleted() " )

    }


}
