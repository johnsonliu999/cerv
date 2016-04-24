#ifndef CSERIALREADER_H
#define CSERIALREADER_H
#include <QSerialPort>
#include <cv.h>
#include <QString>
#include <QList>

//! bluetooth device id.
/*! detail description */
#define BT_ID 8963
#define ARDUINO_ID 67
#define BT_ADDR "0x00158300428D"


/// 单例模式： 串口通信类（只读）
/// 使用方法： 先获取单例，open（）--> connectDevice() --> readSerial -->closeSerial().
/// 唯一有用的接口： readSerial（）
/// 所有错误都以异常抛出

///
/// \brief The CSerialReader class
/// provides access to the serial port
///
class CSerialReader
{
public:
    static CSerialReader* getReader();
    bool OpenSerial(int nProductID = BT_ID);
    void CloseSerial();
    const QList<QList<int> > ReadSerial();
    void ConnectDevice(const QString & cDevAddr = "0x00158300428D");
    bool isConnected();

private:
    QSerialPort m_iPort; ///< store current port.
    static CSerialReader* m_pReader; ///< singleton mode, store CSerialReader object.

private:
    CSerialReader(); ///< private constructor. Since singleton mode.
    const QList<QList<int> > __ParseData(const QStringList & iStringList);

};

#endif // CSERIALREADER_H
