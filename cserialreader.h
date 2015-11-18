#ifndef CSERIALREADER_H
#define CSERIALREADER_H
#include <QSerialPort>
#include <cv.h>
#include <QString>
#include <QList>

#define BT_ID 8963
#define ARDUINO_ID 67
#define BT_ADDR "0x00158300428D"

class CSerialReader
{
public:
    static CSerialReader* getReader();
    bool OpenSerial(int nProductID = BT_ID);
    void CloseSerial();
    const QList<QList<int> > ReadSerial();
    bool ConnectDevice(const QString & cDevAddr = "0x00158300428D");
    bool isConnected();

private:
    QSerialPort m_iPort;
    static CSerialReader* m_pReader;

private:
    CSerialReader();
    const QList<QList<int> > __ParseData(const QStringList & iStringList);

};

#endif // CSERIALREADER_H
