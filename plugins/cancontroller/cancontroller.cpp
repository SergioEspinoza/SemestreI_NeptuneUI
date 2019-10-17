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



#include "cancontroller.h"



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


void CanController::devFramesReceived( void )
{
    QMap<QString, QCanBusDevice* >::iterator i;
    QCanBusDevice* canDev;
    QCanBusFrame frame;
    QVariantMap mapFrame;
    QList< QCanBusFrame >::iterator cacheIterator;
    bool idMatch = false;


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

                idMatch = false;
                //look for previous frame
                for( cacheIterator =  m_messageCache[ i.key() ].begin() ;
                     cacheIterator != m_messageCache[ i.key() ].end() ; ++cacheIterator )
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


                            emit rxMessageDataChanged( i.key(), mapFrame );
                        }
                    }
                }

                //first time receive, add to cache, emit signal as well
                if( !idMatch )
                {
                    m_messageCache[i.key()].append( frame );

                    mapFrame.insert( "frameId", frame.frameId() );
                    mapFrame.insert( "frameType", frame.frameType() );
                    mapFrame.insert( "payload", frame.payload() );

                    //cannot map QFlags<FrameError> type
                    // TODO: find another way
                    //mapFrame.insert( "error", frame.error() );


                    emit rxMessageDataChanged( i.key(), mapFrame );

                }

            }
        }

    }
}


void CanController::devFramesWritten( qint64 framesWritten )
{
    Q_UNUSED( framesWritten )

}


void CanController::devErrorOccurred( QCanBusDevice::CanBusError error )
{

    //TODO: translate error and log into file
    qDebug() << "CAN bus error !! ";


    qDebug() << error;

}


int CanController::readCanConfigFile( QString devName, QString filename )
{
    int ret = CANCONTROLLER_ERROR;
    QJsonParseError error;
    QFile jsonFile( filename );
    QByteArray fileData;

    if( jsonFile.open(QIODevice::ReadOnly ) )
    {
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
                ret = parseJsonMsgObj( devName, jObj );
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

int CanController::parseJsonMsgObj ( QString devName, QJsonObject obj )
{
    int ret = CANCONTROLLER_ERROR;
    QList<QCanBusDevice::Filter> filters;
    QCanBusDevice::Filter tmpFilter;

    Q_ASSERT( devName != "" );


    QJsonArray jArr = obj[ "messages" ].toArray();


    // ONLY CONFIGURE FILTERS FOR NOW

    for( int i = 0 ; i < jArr.size() ; i++ )
    {
        if( obj.contains( "id" ) )
        {
             tmpFilter.frameId = (quint32)obj[ "id" ].toInt();
             //only listen to specific id
             tmpFilter.frameIdMask = 0x3FF;
             tmpFilter.type = QCanBusFrame::DataFrame;

             filters.append( tmpFilter );

             //at least one message configured
             ret = CANCONTROLLER_SUCESS;

        }
        else
        {
            //not possible to parse message
            qDebug( ) << "cannot parse message";
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

