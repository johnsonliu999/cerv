#include "cserialreader.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include <QSqlDatabase>
#include <QThread>
#include <QByteArray>



///
/// \brief CSerialReader::CSerialReader
/// \param portName
///
CSerialReader::CSerialReader(const QString &portName) :
    mp_port(new QSerialPort(portName))
{
    mp_port->setBaudRate(9600);
    mp_port->setDataBits(QSerialPort::Data8);
    mp_port->setParity(QSerialPort::NoParity);
    mp_port->setStopBits(QSerialPort::OneStop);
}

CSerialReader::~CSerialReader()
{

}

///
/// \brief CSerialReader::getPort
///
/// \exception <QString> {}
///  \return
///
QSerialPort* CSerialReader::openPort() const
{
    if (mp_port->isOpen())
        throw QString("Port has been opened by other function.");

    if (!mp_port->open(QSerialPort::ReadWrite))
        throw QString("getPort : ") + mp_port->errorString();

    return mp_port;
}

///
/// \brief CSerialReader::ConnectDevice
/// \param cDevAddr address of being connected device.
/// The device should work under slave mode.
///
/// \exception <QString> {throwed by serial port}
///
void CSerialReader::ConnectDevice(const QString &cDevAddr)
{
    qDebug() << "Going to connect device at " + cDevAddr;

    if (isConnected())  return;

    /*** connect device ***/
    qDebug() << "Device found, try to connect";
    // try to connect the device
    QSerialPort* p_port = openPort();

    try{
        if (-1 == p_port->write("at+cona"+cDevAddr.toLatin1()+"\r\n"))
           throw QString("Error write conna");

        qDebug() << "Wait for receive cona info";
        if (!p_port->waitForReadyRead(2000))
            throw QString("long wait");

    } catch (const QString& e)
    {
        p_port->close();
        throw e;
    }
    p_port->close();
}

///
/// \brief CSerialReader::isConnected
/// \return
///
bool CSerialReader::isConnected()
{
    QSerialPort* p_port = openPort();

    bool f = mp_port->waitForReadyRead(2000);

    p_port->close();

    return f;
}

///
/// \brief CSerialReader::findDev
///
/// \exception <QString> {}
///
/// \return
///
QList<QString> CSerialReader::findDev()
{
    if(isConnected())
        throw QString("Device has been connected.");

    // write inquery and receive response.
    QSerialPort* p_port = openPort();
    try{
        if (-1 == p_port->write("at+inq\r\n"))
            throw QString("Port write error.");

        if (!p_port->waitForReadyRead(2000))
            throw QString("Time out");

    } catch(const QString& e)
    {
        p_port->close();
        throw e;
    }

    QByteArray res;
    while (p_port->waitForReadyRead(5000))
        res += mp_port->read(100);

    p_port->close();

    // analyse response.
    if (!res.contains("INQE"))
        throw QString("Not find inquery end.");

    QList<QString> devList;
    QRegExp rx(":\\d\\s(0x[0-9A-Z]{12})");
    //rx.setMinimal(true);
    int pos = 0;
    while ((pos = rx.indexIn(QString(res), pos)) != -1)
    {
        devList << rx.cap(1);
        pos += rx.matchedLength();
    }

    return devList;
}

/**
 * @brief CSerialReader::__ParseData
 * 对发送的QStringList数据进行解析，生成int型的QList
 *
 * @param iStringList：发送的QStringList类型数据
 * @return：QList的int型数据
 */
const QList< QList<int> > CSerialReader::parseRecords(const QStringList & iStringList)
{
    QList< QList<int> > iDataList;
    QList<int> iIntList;

    for (int i = 0; i < iStringList.size(); i++)
    {
        // 生成split后的String类型的数据链
        QStringList iStringItemList = iStringList[i].split("\t");
        // 去除开头的#和结尾的*
        iStringItemList.removeFirst();
        iStringItemList.removeLast();

        // 每次使用前清空iIntList
        iIntList.clear();
        for (int j = 0; j < iStringItemList.size(); j++)
        {
            iIntList << iStringItemList[j].toInt();
        }
        // 再将iIntList插入iDataList
        iDataList << iIntList;
    }

    return iDataList;
}


///
/// \brief CSerialReader::getTrainData
/// 2000 bytes will cost around 2 secs
///
///
///  \return
///
const QList< QList<int> > CSerialReader::getTrainData() const
{
    QStringList records;

    // 读取前需要设置阻塞
    if (!mp_port->waitForReadyRead(5000))
        throw QString("No data read in 5s");

    QByteArray array;
    while(array.length() < 2000)
    {
        array += mp_port->readAll();
        mp_port->waitForReadyRead(-1);
    }

//    qDebug() << "Raw Data" << array.length() << ":" << array.data();

    // 提取完整数据，需要满足以#开头，*结尾，20个数据
    QRegExp rx("(#\\s(\\d{1,4}\\s){20}\\*\r\n)");
    rx.setMinimal(true); // 设置非贪心模式，匹配最小长度

    int pos = 0;
    // rx.indexIn返回的在str中的位置
    while ( (pos < array.length()) &&
            ((pos = rx.indexIn(QString(array), pos)) != -1) )
    {
        // 往iList中插如第一个括号匹配的内容
        records << rx.cap(1);
        pos += rx.matchedLength(); // 移动开始匹配模式的位置，避免重复匹配
    }

    QList<QList<int> > data = parseRecords(records);

    return data;
}

