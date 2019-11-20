#ifndef JSONMESSAGE_H
#define JSONMESSAGE_H

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QDebug>

#include <jsonsignal.h>

class JsonMessage : public QObject
{
    Q_OBJECT
public:
    explicit JsonMessage(QObject *parent = nullptr);

    JsonMessage(QJsonObject obj);
    JsonMessage(const JsonMessage &m2);
    JsonMessage& operator=(const JsonMessage &);


    //Getters
    QString getName() { return name; }
    qint32 getId() { return id; }
    qint32 getDlc() { return dlc; }
    QByteArray getPayload() { return payload; }
    QVector<JsonSignal> getSignals() { return sgnls; }

signals:

public slots:

private:
    QString name = "";
    qint32 id = -1;
    qint32 dlc = -1;
    QByteArray payload;
    QVector<JsonSignal> sgnls;

};

#endif // JSONMESSAGE_H
