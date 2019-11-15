/**
* YAZAKI COPYRIGHT
*/


#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusDevice>
#include <QtSerialBus/QCanBusFrame>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QtSerialPort/QSerialPort>

#include "cancontroller.h"
#include "jsonmessage.h"

static QVector<JsonMessage> messages;

CanController::CanController(QObject *parent)
    : QObject(parent)
{

    //loads available pluginsw
     m_canBus = QCanBus::instance();

}


int CanController::canInit( QString devName, int bitrate )
{
    int ret = CANCONTROLLER_ERROR;
    QString errorMsg;
    QList< QCanBusDeviceInfo > deviceList;
    QCanBusDevice* canBusDevice;


    if( m_canBus->plugins().contains( QStringLiteral( "socketcan" ) ) )
    {
        deviceList = m_canBus->availableDevices("socketcan", &errorMsg );

       if( errorMsg.isEmpty() )
       {
           //TODO: check for devnName present in deviceList
           canBusDevice = m_canBus->createDevice( "socketcan", devName, &errorMsg );

           if( errorMsg.isEmpty() )
           {
               m_canBusDevices.insert( devName, canBusDevice );

               qDebug() << "CAN bus device " + devName + " created";

               connect(canBusDevice, &QCanBusDevice::errorOccurred,
                       this,  &CanController::devErrorOccurred );               
               connect(canBusDevice, &QCanBusDevice::framesReceived,
                       this, &CanController::devFramesReceived );
               connect(canBusDevice, &QCanBusDevice::framesWritten,
                       this, &CanController::devFramesWritten );

               canBusDevice->setConfigurationParameter( QCanBusDevice::BitRateKey, bitrate );

               ret = CANCONTROLLER_SUCESS;
           }
           else
           {
               qDebug() << errorMsg;
               qDebug() << "unable to create CAN " + devName + " device";
           }
       }
       else
       {
           qDebug() << errorMsg;
           qDebug() << "CAN device name " + devName + " not found";

       }


    }
    else
    {
        qDebug() << "socket can plugin unavailable";

        ret = CANCONTROLLER_ERROR;

    }

    return ret;

}

Q_INVOKABLE int CanController::amp_canInit( QString devName )
{
    int ret = CANCONTROLLER_ERROR;
    QSerialPort* serialPort;

    serialPort = new QSerialPort( devName );

    if( serialPort->open( QSerialPort::ReadWrite )  )
    {
        qWarning() << "unable to open serial channel";
    }
    else
    {

        m_ampChannel = serialPort;
        ret = CANCONTROLLER_SUCESS;
    }

    return ret;
}


int CanController::amp_canInitRx( void )
{
    int ret = CANCONTROLLER_ERROR;

    // thos implementation asumes a dedicated channel for reading can
    // input related messages.
    // In case this apprach is not possible, message read should not be
    // made here (through QSerialPort signal) but instead have a
    // dedicated AMP message controller / router as a separate
    // plugin component and have it route json object here
    //


    if( ( m_ampChannel != nullptr ) &&
            ( m_ampChannel->isOpen() ) )
    {
        QObject::connect( m_ampChannel, &QSerialPort::channelReadyRead , this, &CanController::ampReadyRead );

    }
    else
    {
        qWarning() << "amp channel not ready ";
    }

    return ret;
}



void CanController::devSignalChanged(QCanBusFrame frame)
{
    QByteArray payload = frame.payload();
    QString signalNames[] = {   "hill_descent_control_id", "intelligent_speed_adaptation_id",
                                "automatic_beam_switching_id", "collision_avoidance_id",
                                "lane_assist_id", "traffic_jam_assist_id",
                                "driver_drowsiness_alert_id", "park_assist_id"};

    // Definition of signal using a Byte.
    if( ( (frame.payload()[0] )&0x08) == 0x08){     // 4th bit shows a signal is activated.
        // Selection of signal
        if( ( (payload[0] )&0x04) == 0x04){
            if( ( (payload[0] )&0x02) == 0x02){
                if( ( (payload[0] )&0x01) == 0x01){
                    emit rxSignalValueChanged(signalNames[7], 15);
                } else {
                    emit rxSignalValueChanged(signalNames[6], 14);
                }
            } else {
                if( ( (payload[0] )&0x01) == 1){
                    emit rxSignalValueChanged(signalNames[5], 13);
                } else{
                    emit rxSignalValueChanged(signalNames[4], 12);
                }
            }
        }else {
            if( ( (payload[0] )&0x02) == 0x02){
                if( ( (payload[0] )&0x01) == 0x01){
                    emit rxSignalValueChanged(signalNames[3], 11);
                } else {
                    emit rxSignalValueChanged(signalNames[2], 10);
                }
            } else {
                if( ( (payload[0] )&0x01) == 1){
                    emit rxSignalValueChanged(signalNames[1], 9);
                } else{
                    emit rxSignalValueChanged(signalNames[0], 8);
                }
            }

        }
    } else {                                    // 4th bit shows a signal is deactivated.
        // Selection of signal
        if( ( (payload[0] )&0x04) == 0x04){
            if( ( (payload[0] )&0x02) == 0x02){
                if( ( (payload[0] )&0x01) == 0x01){
                    emit rxSignalValueChanged(signalNames[7], 7);
                } else {
                    emit rxSignalValueChanged(signalNames[6], 6);
                }
            } else {
                if( ( (payload[0] )&0x01) == 1){
                    emit rxSignalValueChanged(signalNames[5], 5);
                } else{
                    emit rxSignalValueChanged(signalNames[4], 4);
                }
            }
        }else {
            if( ( (payload[0] )&0x02) == 0x02){
                if( ( (payload[0] )&0x01) == 0x01){
                    emit rxSignalValueChanged(signalNames[3], 3);
                } else {
                    emit rxSignalValueChanged(signalNames[2], 2);
                }
            } else {
                if( ( (payload[0] )&0x01) == 1){
                    emit rxSignalValueChanged(signalNames[1], 1);
                } else{
                    emit rxSignalValueChanged(signalNames[0], 0);
                }
            }

        }
    }
}


void CanController::devFramesReceived( void )
{
    QMap<QString, QCanBusDevice* >::iterator i;
    QCanBusDevice* canDev;
    QCanBusFrame frame;

    // check for received frames on all buses
    for( i = m_canBusDevices.begin() ; i != m_canBusDevices.end() ; ++i  )
    {
        canDev = i.value();

        while( ( canDev->state() == QCanBusDevice::ConnectedState ) &&
               (  canDev->framesAvailable() > 0 ) )
        {
            //unqueue frame by frame
            frame = canDev->readFrame( );



            if( ( frame.isValid() ) && ( frame.error() == QCanBusFrame::NoError ) )
            {
                processIncomingCanMsg( i.key(), frame );
            }
        }

    }
}


void CanController::processIncomingCanMsg( QString chanName, QCanBusFrame& frame )
{
        QList< QCanBusFrame >::iterator cacheIterator;
        QVariantMap mapFrame;
        bool idMatch = false;

        //look for previous frame
        for( cacheIterator =  m_messageCache[ chanName ].begin() ;
             cacheIterator != m_messageCache[ chanName ].end() ; ++cacheIterator )
        {
            if( cacheIterator->frameId() == frame.frameId() )
            {
                //msg found compare to last recorded value
                idMatch = true;
                if( cacheIterator->payload() != frame.payload()  )
                {
                    //update frame
                    *cacheIterator = frame;

                    mapFrame.insert( "frameId", frame.frameId() );
                    mapFrame.insert( "frameType", frame.frameType() );
                    mapFrame.insert( "payload", frame.payload() );

                    //cannot map QFlags<FrameError> type
                    // TODO: find another way
                    //mapFrame.insert( "error", frame.error() );


                    devSignalChanged(frame);
                    emit rxMessageDataChanged( chanName, mapFrame );
                }
            }
        }

        //first time receive, add to cache, emit signal as well
        if( !idMatch )
        {
            m_messageCache[chanName].append( frame );

            mapFrame.insert( "frameId", frame.frameId() );
            mapFrame.insert( "frameType", frame.frameType() );
            mapFrame.insert( "payload", frame.payload() );

            //cannot map QFlags<FrameError> type
            // TODO: find another way
            //mapFrame.insert( "error", frame.error() );

            devSignalChanged(frame);
            emit rxMessageDataChanged( chanName, mapFrame );
        }
}




void CanController::devFramesWritten( qint64 framesWritten )
{
    Q_UNUSED( framesWritten )

}


void CanController::devErrorOccurred( QCanBusDevice::CanBusError error )
{

    //TODO: translate error and log into file
    qWarning() << "CAN bus error !! ";


    qWarning() << error;

}


void  CanController::ampReadyRead( int channel )
{
    Q_UNUSED( channel )

    QByteArray readBuffer;
    QJsonParseError jsonerror;
    QJsonDocument jsonDoc;

    if( m_ampChannel->canReadLine( ) == true )
    {
        //json document should be 1 line
         readBuffer = m_ampChannel->readLine( );

         jsonDoc = QJsonDocument::fromJson( readBuffer, &jsonerror );

         if( jsonerror.error != QJsonParseError::NoError)
         {
             if ( ampDeserializeMsg( jsonDoc.object() ) != CANCONTROLLER_SUCESS )
             {
                 qWarning() << "AMP can message parse error";
             }

         }
         else
         {
             qWarning() << "AMP can message json format error";
         }
    }
}

int CanController::ampDeserializeMsg( QJsonObject obj )
{
    QString msgType;
    QVariantMap msg;
    QJsonArray::iterator iterator;
    QCanBusFrame frame;
    QJsonArray jArray;
    QJsonObject curObj;
    QByteArray payload;
    int ret = CANCONTROLLER_ERROR;



    msgType = obj[ "ampMsgType" ].toString();

    //asume only CAN messages for now
    if(  ( msgType == "can") &&
            obj.contains( "messages" ) )
    {


        jArray = obj["messages"].toArray();

        for( QJsonArray::iterator i = jArray.begin() ; i != jArray.end() ; i++ )
        {

            if( i->isObject() )
            {
                curObj = i->toObject();

                //TODO: check conversion
                payload =  QByteArray::fromHex( curObj["payload"].toString().toUtf8() );

                frame.setPayload( payload );
                //frame
                frame.setFrameId( (quint32) curObj["msgId"].toInt() );

                processIncomingCanMsg( "AMP", frame );

                ret = CANCONTROLLER_SUCESS;
           }

        }

    }

    return ret;
}


int CanController::readCanConfigFile( QString devName, QString filename )
{
    int ret = CANCONTROLLER_ERROR;
    QJsonParseError error;

    // Note: to use the ":/filename" notation, the file has to be added to the .qrc resources file.
    QFile jsonFile( ":/" + filename );
    QByteArray fileData;

    if( jsonFile.open(QIODevice::ReadOnly ) )
    {
        // Should this warning be here???
        qWarning() << "Unable to open filter definition file";

        fileData = jsonFile.readAll();

        QJsonDocument jDoc = QJsonDocument::fromJson( fileData, &error ) ;

        if( error.error != QJsonParseError::NoError )
        {
            //something went wrong during JSON parse
            qWarning() << " Error during json filter definition file parse! ";
            qWarning() << " error: " + error.errorString();
        }
        else
        {

        // get all ids
            QJsonObject jObj = jDoc.object();

            if( jObj.contains( "messages" ) )
            {
                ret = parseJsonMsgConfigObj( devName, jObj );
            }
        }

        if( ret == CANCONTROLLER_SUCESS )
        {
            //set own filters filters
            setFilters( devName, m_canFilters[devName] );
        }
    }

    return ret;

}


Q_INVOKABLE int CanController::initCanRx( QString devName )
{
    QCanBusDevice* canDev = nullptr;
    int ret = CANCONTROLLER_ERROR;

    if( m_canBusDevices.contains( devName ) )
    {
        canDev = m_canBusDevices[ devName ];

        if( canDev->connectDevice() )
        {
            ret = CANCONTROLLER_SUCESS;
        }
        else
        {
            qWarning() << "unable to connect CAN " + devName + " device";

        }

    }

    return  ret;
}

int CanController::parseJsonMsgConfigObj ( QString devName, QJsonObject obj )
{
    int ret = CANCONTROLLER_ERROR;
    QList<QCanBusDevice::Filter> filters;
    QCanBusDevice::Filter tmpFilter;

//    // Temporal vector to visualize messages during Debug.
//    QVector<JsonMessage> messages;

    Q_ASSERT( devName != "" );


    QJsonArray jArr = obj[ "messages" ].toArray();


    // ONLY CONFIGURE FILTERS FOR NOW

    for( int i = 0 ; i < jArr.size() ; i++ )
    {
        QJsonObject jObj = jArr[i].toObject();
        QString name;
        if( jObj.contains("id") )
        {
            JsonMessage tempMessage(jObj);

            tmpFilter.frameId = (quint32)tempMessage.getId();
            //only listen to specific id
            tmpFilter.frameIdMask = 0x3FF;
            tmpFilter.type = QCanBusFrame::DataFrame;

            filters.append( tmpFilter );

            //at least one message configured
            ret = CANCONTROLLER_SUCESS;

            messages.append(tempMessage);
        }
        else
        {
            //not possible to parse message
            qDebug( ) << "cannot parse database config mask";
        }
    }

    //TODO: below statement should store into
    // return value rather that directly
    m_canFilters.insert( devName, filters );

    //TODO: Implement complete message object parsing (name, cycle, signals, signal offsets etc.)

    return ret;
}



int CanController::setFilters( QString devName, QList< QCanBusDevice::Filter >& filters )
{
    int ret = CANCONTROLLER_ERROR;
    QCanBusDevice* canDev = nullptr;

    if( ( canDev = m_canBusDevices[ devName ] ) != nullptr )
    {
        canDev->setConfigurationParameter( QCanBusDevice::RawFilterKey, QVariant::fromValue( filters ) );
        ret = CANCONTROLLER_SUCESS;
    }

    return ret;
}

