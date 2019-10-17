/****************************************************************************
**  YAZAKI COPYRIGHT
**
****************************************************************************/

import QtQuick 2.0
import QtApplicationManager 1.0


/*!
    \qmltype CanNotificationInterface
    \inqmlmodule service
    \inherits Notification
    \brief An interface for requesting notifications of CAN input changes.

    NotificationInterface inherits \l {Notification} from \l{Qt Application Manager}
    and acts like an adapter for Neptune UI.
    The NotificationInterface properties \c timeout and \c category are adopted
    to match Neptune UI requirements and should not be changed.
    
    Incoming CAN messages have a single source which is CanModel.qml
    Outgoing CAN messages (usually user setting) will have single endpoint
    which is the same CanModel.qml type


    \section2 Example Usage

    The following example demonstrates the NotificationInterface usage
    for subscribing to a CAN message

    \qml

    NotificationInterface {
        summary: "can message"
        priority: 9

    canId: 0x211
        data:

    }
    \endqml

    \c summary defines a notification title. \c body defines a notification
    description. Notifications are queued based on a value in \c priority.
    \c icon must contain an absolute icon path. Otherwise, the icon is not shown.
*/

Notification {



    id: root
    timeout: 0
    category: "can"


    /*!
         \qmlproperty string summary

         set to either:
             - "message" : transmit payload associated with msgId
             - "signal" :
             - "error" remote frame identifier
    */
    summary: "unset"


    /*!
      * body can be used to identify CAN error messages
      * this is a textual message
      */
    body: "unset"


    /*!
      \qmlproperty canBus
           bus name (i.e. MCAN / CCAN etc)
      */
    property string canBus: "unset"

    /*!
         \qmlproperty string canId

         This property set CAN message frame ID.
         It also serves to prioritize messages on the CAN bus,
         which is independent of notification priority
    */

    property int canId: 0

    /*!
         \qmlproperty int payload

    holds 64 bit (max) CAN frame payload or
    signal value

    */
    property var payload: [0,0,0,0]


    /*!
      \qmlproperty signalName
      */
    property string signalName: "unset"



    /*!
         \qmlproperty enum FrameType

           frame type being received / sent,
           not used because of redundant with
           message
    */
    //    enum FrameType {
    //        Unset,
    //        Data,
    //        Remote,
    //        Error,
    //        Max
    //    }


    //    property int frameType : CanNotificationInterface.FrameType.Unset

    onCanBusChanged: {
        updateData()
    }

    onCanIdChanged: {
        updateData()
    }

    onPayloadChanged: {
        updateData()
    }

    //    onFrameTypeChanged: {
    //        updateDate()
    //    }

    function updateData( )
    {
        /*!
              extended property for CAN frame data
              store id / payload for now.
              TODO: implement extra fields for individual signals, describe
              using startbit, size etc.
          */

        extended = {
            "canBus" : root.canBus,
            "frameType" : "unset", //root.frameType,
            "canId" : root.canId,
            //unable to propagate "payload" as Int8Array objet
            "payload" : root.payload,
            "signalName" : root.signalName
        }
    }

}
