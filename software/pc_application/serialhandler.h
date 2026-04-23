/**
 * @file serialhandler.h
 * @brief Declaration of the SerialHandler class.
 *
 * SerialHandler manages asynchronous communication with the physical controller over a virtual COM port.
 */

#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

/**
 * @class SerialHandler
 * @brief Receives and parses sensor data frames from the microcontroller.
 *
 * The controller sends text frames at 50 Hz in the format:
 * @code
 * $AD,<measured_dist>,<compensated_dist>,<acc_deg>,<gyro_deg>,<angle_deg>\n
 * @endcode
 * SerialHandler opens the serial port, buffers incoming bytes until a full line is available, validates the frame prefix,
 * converts each field to a float, and emits the dataReceived() signal with theparsed values.
 */
class SerialHandler: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the SerialHandler and configures port parameters.
     * @param parent - Optional parent QObject.
     */
    SerialHandler(QObject *parent = nullptr);

    /**
     * @brief SerialHandler's destructor.
     */
    ~SerialHandler();

    /**
     * @brief Opens the serial port for reading.
     * @param portName - System name of the port.
     * @return true if the port was opened successfully, false if not.
     */
    bool openPort(const QString &portName);

    /**
     * @brief Closes the serial port if it is currently open.
     * @return true if the port was open and is now closed, false if it was already closed.
     */
    bool closePort();

    /**
     * @brief Provides a list with available serial ports.
     * @return portNames - list with available serial ports.
     */
    QStringList availablePorts();

private:
    /**
     *  @brief QSerialPort instance used for communication.
      */
    QSerialPort *serial;

private slots:
    /**
     * @brief Slot connected to QSerialPort::readyRead.
     *
     * Reads all available complete lines from the serial buffer, validates the prefix and the expected number of fields,
     * converts each field to float, and emits dataReceived() on success.
     */
    void onDataReady();

signals:
    /**
     * @brief Emitted when a valid sensor data frame has been parsed.
     * @param measuredDist - Raw distance measurement from distance sensor [mm].
     * @param realDist - Compensated distance [mm].
     * @param accDeg - Roll angle computed from accelerometer [degrees].
     * @param gyroDeg - Roll angle integrated from gyroscope [degrees].
     * @param angleDeg - Roll angle after complementary filter [degrees].
     */
    void dataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg);

};

#endif // SERIALHANDLER_H
