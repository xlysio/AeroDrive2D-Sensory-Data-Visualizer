#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialHandler: public QObject
{
    Q_OBJECT

public:
    SerialHandler(QObject *parent = nullptr);
    ~SerialHandler();
    bool openPort(const QString &portName);
    bool closePort();

private:
    QSerialPort *serial;

private slots:
    void onDataReady();

signals:
    void dataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg);

};

#endif // SERIALHANDLER_H
