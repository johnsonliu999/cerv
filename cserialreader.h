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
/// provides access to serial port and formed data for other classes.
/// Since the accuracy of serial port seriously decreases due to unknown beginning problem,
/// the openning and closing operation are executed by the caller.
///
/// 1. All function will throw exception.\n
/// 2. When being initialized, [portName] should be given. [portName] is consider
/// to be a constant.\n
/// 3. When open serial port, use [CSerialReader.getPort] rather than [QSerialPort.open].
/// bedause [openPort] handle some exceptions to open serial port.
///
class CSerialReader
{

private:
    QSerialPort* mp_port; ///< store current port.

public:
    CSerialReader(const QString& portName);
    ~CSerialReader();

    void openPort() const;
    void closePort();
    void ConnectDevice(const QString & cDevAddr = "0x00158300428D");
    bool isConnected();
    QList<QString> findDev();
    const QList<QList<int> > getTrainData() const;

private:
    static const QList<QList<int> > parseRecords(const QStringList & iStringList);

};

#endif // CSERIALREADER_H
