#include "jsonsignal.h"

JsonSignal::JsonSignal(QObject *parent) : QObject(parent)
{

}

JsonSignal::JsonSignal(QJsonObject obj, qint32 signId) {
    QJsonObject::iterator k;
    QString sigName = "";
    qint32 sigLength = 0;
    qint32 sigFactor = 0;
    qint32 sigOffset = 0;
    for(k = obj.begin(); k != obj.end(); ++k){
        if ( k.key() == "name"){
            sigName = k.value().toString();
        }
        if ( k.key() == "bit_length"){
            sigLength = k.value().toInt();
        }
        if ( k.key() == "factor"){
            sigFactor = k.value().toInt();
        }
        if ( k.key() == "offset"){
            sigOffset = k.value().toInt();
        }
    }
    this->id = signId;
    this->name = sigName;
    this->bitLength = sigLength;
    this->factor = sigFactor;
    this->offset = sigOffset;
}

JsonSignal& JsonSignal::operator=(const JsonSignal &s2) {
    this->name = s2.name;
    this->id = s2.id;
    this->bitLength = s2.bitLength;
    this->factor = s2.factor;
    this->offset = s2.offset;
    return *this;
}

JsonSignal::JsonSignal(const JsonSignal &s2) {
    name = s2.name;
    id = s2.id;
    bitLength = s2.bitLength;
    factor = s2.factor; offset = s2.offset;
}

