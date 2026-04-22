/**
 * @file serialhandler.cpp
 * @brief Implementation of the SerialHandler class.
 */
#include "serialhandler.h"

SerialHandler::SerialHandler(QObject *parent): QObject(parent)
{
    serial = new QSerialPort(this);

    // port configuration mathching the microcontoller settings
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(serial, &QSerialPort::readyRead, this, &SerialHandler::onDataReady);
}

SerialHandler::~SerialHandler()
{

}

bool SerialHandler::openPort(const QString &portName)
{
    serial->setPortName(portName);

    if (!serial->open(QIODevice::ReadOnly))
    {
        return false;
    }

    return true;
}

bool SerialHandler::closePort()
{
    if (serial->isOpen())
    {
        serial->close();
        return true;
    }

    return false;
}

void SerialHandler::onDataReady()
{
    while (serial->canReadLine())
    {
        QString line = QString::fromUtf8(serial->readLine()).trimmed();

        // check frame prefix
        if (!line.startsWith("$AD,"))
        {
            continue;
        }

        // remove prefix and split into fields
        line = line.mid(4);
        QStringList data = line.split(",");

        // validate amount of fields
        if (data.size() != 5)
        {
            continue;
        }

        // convert fields to floats
        bool ok0, ok1, ok2, ok3, ok4;
        float measuredDist = data[0].toFloat(&ok0);
        float realDist = data[1].toFloat(&ok1);
        float accDeg = data[2].toFloat(&ok2);
        float gyroDeg = data[3].toFloat(&ok3);
        float angleDeg = data[4].toFloat(&ok4);

        // emit signal only if all conversions succeeded
        if (ok0 && ok1 && ok2 && ok3 && ok4)
        {
            emit dataReceived(measuredDist, realDist, accDeg, gyroDeg, angleDeg);
        }
    }
}