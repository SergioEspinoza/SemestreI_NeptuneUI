#ifndef JSONSIGNAL_H
#define JSONSIGNAL_H

#include <QObject>
#include <QJsonObject>

/*!
 * \brief The JsonSignal class
 */

class JsonSignal : public QObject
{
    Q_OBJECT

public:
    explicit JsonSignal(QObject *parent = nullptr);

    JsonSignal(QJsonObject, qint32);
    JsonSignal(const JsonSignal &s2);
    JsonSignal& operator=(const JsonSignal &);

    // Getters
    qint32 getId() { return id; }
    QString getName() { return name; }
    qint32 getBitLength() { return bitLength; }
    qint32 getFactor() { return factor; }
    qint32 getOffset() { return offset; }

//    //Setters
//    void setId(qint32);
//    void setName(QString);
//    void setBitLength(qint32);
//    void setFactor(qint32);
//    void setOffset(qint32);


signals:


private slots:


private:
    qint32 id = -1;
    QString name = "";
    qint32 bitLength = -1;
    qint32 factor = -1;
    qint32 offset = -1;

};

#endif // JSONSIGNAL_H
