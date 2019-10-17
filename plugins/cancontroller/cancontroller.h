/**
* YAZAKI COPYRIGHT
*/


#ifndef CANCONTROLLER_H_
#define CANCONTROLLER_H_

#include <QtCore/QObject>
#include <QQueue>
#include <QList>
#include <QMap>
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusDevice>


#define CANCONTROLLER_SUCESS   0
#define CANCONTROLLER_ERROR    -1

/*!
 * \brief The CanController class
 */

class CanController : public QObject
{
    Q_OBJECT

public:
    explicit CanController(QObject *parent = nullptr);


    /*!
     * \brief CanController::canInit
     * \param devName
     * \param bitrate
     * \return
     */
    Q_INVOKABLE int canInit( QString devName, int bitrate );

    /*!
     * \brief CanController::readCanConfigFile
     *   for now, only set filters according to defined messages
     * \param devName
     * \param filename
     * \return
     */
    Q_INVOKABLE int readCanConfigFile( QString devName, QString filename );


    /*!
     * \brief initCanRx
     * clears can buffer / queue, set filters
     * filters for the given channel. Case no filters
     * are set all messages will be listened to
     * and will generate rxDataChanged /
     * rxSignalValueChanged
     *
     *
     * \return
     */
    Q_INVOKABLE int initCanRx( QString devName );


    /*!
     * \brief setFilters
     *
     * set up filters for given CAN channel
     *
     * \param devName
     * \param filters
     * \return
     */
     Q_INVOKABLE int setFilters(QString devName, QList<QCanBusDevice::Filter> &filters );


signals:

    /*!
     * \brief rxMessageDataChanged
     *  message payload related to given id changed from the last
     *  received
     * \param devName CAN bus interface name source (i.e. can0, vcan0, etc.)
     * \param frame QVariantMap with following fields:
     *          QVariantMap["frameId"] : CAN message frame ID (32 bit integer value)
     *          QVariantMap["frameType"]: CAN message frame enum type (enum QCanBusFrame::FrameType)
     *          QVariantMap["payload"]: CAN payload byte array (QByteArray)
     *                 TODO: QVariantMap["error"] NOT YET IMPLEMENTED
     *
     */
    void rxMessageDataChanged( QString devName, QVariantMap frame );


    /*!
     * \brief rxSignalValueChanged
     * rx singal value changed from the last received
     *
     * \param signalName signal name
     * \param value signal value
     *
     * TODO: Implement!!!
     */
    void rxSignalValueChanged( QString signalName, quint32 value);

private slots:

    /*!
     * \brief devFramesReceived
     * slot for QCanBusDevice::framesReceived
     * frames received on active CanBusDevice
     */
    void devFramesReceived( void );

    /*!
     * \brief devFramesWritten
     * frames written on active CanBusDevice
     */
    void devFramesWritten( qint64 framesWritten );

    /*!
     * \brief devErrorOccurred
     * error occurred on active CanBusDevice
     */
    void devErrorOccurred( QCanBusDevice::CanBusError error );


private:



    /*!
     * \brief initCanTx
     * Initiate can message transmit thread.
     * Tx Thread will start cycling tx msg transmission according
     * to preset message cycles
     *
     * TODO: Implement!!
     *
     * \return
     */
    int initCanTx( QString devName );


    /*!
     * \brief parseJsonMsgObj
     * read message parameters and set m_canFilters
     * and others applicable members
     * \param devName  channel nam
     * \param obj  JSON object
     *
     * \return CANCONTROLLER_SUCCESS / FAIL
     */
    int parseJsonMsgObj ( QString devName, QJsonObject obj );




    /*!
     * \brief m_signalCache
     * Holds last known signals values
     * for each configured channel
     *
     * format is: QMap<"channel name" , QMap< "signal name", "value" > >
     * TODO: To be implemented
     */
     QMap< QString, QMap< QString, int > > m_signalCache;

     /**
      * @brief m_messageCache
      * Holds last known message values for each configured channel
      *
      * format is: QMap< "channel name", QList< "CAN messages" > >
      *
      */
     QMap< QString, QList<QCanBusFrame> > m_messageCache;


     /*!
      * \brief m_canBus
      *
      *   can bus singleton
      */
     QCanBus* m_canBus;

     /*!
      * \brief m_canBusDevices
      * Can bus devices, format:
      * QMap< "device name", 'Bus device"  >
      */

     QMap< QString, QCanBusDevice* > m_canBusDevices;

     /*!
      * \brief m_canFilters
      *
      * filters currently associated with each channel.
      *
      * format is: QMap<"channel name", QList< "list of filters" > >
      *
      */
     QMap< QString, QList<QCanBusDevice::Filter> > m_canFilters;

     /*!
      * \brief m_rxQueue
      * stores received can frames ( ** MIGHT NOT BE NEEDED!!!** )
      */
     QQueue< QCanBusFrame > m_rxQueue;


     /*!
      * \brief m_txQueue
      *  cyclic messages can id and data
      * ( beware of asynch access by tx thread! )
      * TODO: implement for cyclic messages
      */
     QQueue< QCanBusFrame > m_txQueue;


};

#endif // CANCONTROlLER_H_
