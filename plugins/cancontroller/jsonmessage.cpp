#include "jsonmessage.h"

JsonMessage::JsonMessage(QObject *parent) : QObject(parent)
{

}

JsonMessage::JsonMessage(QJsonObject obj) {
    QJsonObject jSignalObj;
    bool ok;
    QString idStr= obj.value("id").toString();
    this->id = (qint32)idStr.toInt(&ok, 16);

    if(obj.contains("name")){
        name = obj.value("name").toString();
    } else {
        qDebug() << "message with id " << this->id << " has no name";
    }
    if(obj.contains("signals")){
        jSignalObj = obj["signals"].toObject();
        QJsonObject::iterator j;
        for(j = jSignalObj.begin(); j != jSignalObj.end(); ++j){
            QJsonObject jSign = j.value().toObject();
            qint32 jSignId = j.key().toInt();
            //Signal created
            JsonSignal tempSignal(jSign, jSignId);
            sgnls.append(tempSignal);
        }
    } else {
        qDebug() << "no signals associated to message with id " << id;
    }

}

JsonMessage& JsonMessage::operator=(const JsonMessage &m2) {
    this->name = m2.name;
    this->id = m2.id;
    this->sgnls = m2.sgnls;
    return *this;
}

JsonMessage::JsonMessage(const JsonMessage &m2):QObject() {
    this->name = m2.name;
    this->id = m2.id;
    this->sgnls = m2.sgnls;
}


