#ifndef JSONSIGNAL_H
#define JSONSIGNAL_H

#include <QObject>

class JsonSignal : public QObject
{
    Q_OBJECT
public:
    explicit JsonSignal(QObject *parent = nullptr);

signals:

public slots:
};

#endif // JSONSIGNAL_H
