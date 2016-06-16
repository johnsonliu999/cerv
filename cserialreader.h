#ifndef CSERIALREADER_H
#define CSERIALREADER_H
#include <QSerialPort>
#include "cv.h"
#include <QString>
#include <QList>

//! bluetooth device id.
/*! detail description */
// #define BT_ID 8963

const int BT_ID = 29987;
const int ARDUINO_ID = 67;
const QString BT_ADDR = "0x00158300428D";

///
/// \brief The CSerialReader class
/// provides access to the serial port
///
class CSerialReader
{

private:
    QSerialPort* mp_port; ///< store current port.

public:
    CSerialReader(const QString& portName);
    ~CSerialReader();

    QSerialPort *getPort();
    void ConnectDevice(const QString & cDevAddr = "0x00158300428D");
    bool isConnected();
    QList<QString> findDev();
    const QList<QList<int> > readSerial() const;

private:
    static const QList<QList<int> > __ParseData(const QStringList & iStringList);

};

#endif // CSERIALREADER_H
