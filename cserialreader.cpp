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

CSerialReader* CSerialReader::m_pReader = NULL;

CSerialReader::CSerialReader()
{
}

/**
 * @brief CSerialReader::__ConnectDevice
 * 自动连接程序
 *
 * @param cDevAddr：目标地址，目标需设置为从模式
 * @return：成功为true，失败false
 */
bool CSerialReader::ConnectDevice(const QString &cDevAddr)
{
    qDebug() << "Going to connect device at " + cDevAddr;

    if (isConnected())
    {
        qDebug() << "Already connected";
        return true;
    }

    /*** send inquery device ***/
    qDebug() << "Start inquery and this will last several seconds(at least 5s)";
    if (-1 == m_iPort.write("at+inq\r\n"))
    {
        qDebug() << "Error write";
        return false;
    }

    qDebug() << "Wait for inquery response data";
    if (-1 == m_iPort.waitForReadyRead(2000))
    {
        qDebug() << "No inquery info returned in 2s";
        return false;
    }

    QByteArray iByteArray = "";
    // "INQ" stand for end of inquery

    while (m_iPort.waitForReadyRead(5000))
        iByteArray += m_iPort.read(100);

    qDebug() << iByteArray;

    if (!iByteArray.contains("INQE"))
    {
        qDebug() << "No inquery info returned";
        return false;
    }

    qDebug() << "Received: " << iByteArray;
    if (!iByteArray.contains(cDevAddr.toLatin1()))
    {
        qDebug() << "Device not found";
        return false;
    }


    /*** connect device ***/
    qDebug() << "Device found, try to connect";
    // try to connect the device
    if (-1 == m_iPort.write("at+cona"+cDevAddr.toLatin1()+"\r\n"))
    {
        qDebug() << "Error write conna";
        return false;
    }

    qDebug() << "Wait for receive cona info";
    if (-1 == m_iPort.waitForReadyRead(2000))
        return false;

    return true;

    // may missed the connected info, cause not stop loop
    /* iByteArray = "";
    while (m_iPort.waitForReadyRead(1000))
    {
        iByteArray += m_iPort.read(100);
        if (iByteArray.contains("connected"))
        {
            qDebug() << "Connect success";
            qDebug() << "Returned info: " << iByteArray;
            return true;
        }
    }

    qDebug() << "<connected> not detected, connect failed";
    return false;
    */
}

/**
 * @brief CSerialReader::isConnected
 * assum the serial port has been open
 *
 * @return : Connected-true else false
 */
bool CSerialReader::isConnected()
{
    qDebug() << "In isConnected";
    if(m_iPort.waitForReadyRead(2000))
        return true;
    else
        return false;
}

/**
 * @brief CSerialReader::__ParseData
 * 对发送的QStringList数据进行解析，生成int型的QList
 *
 * @param iStringList：发送的QStringList类型数据
 * @return：QList的int型数据
 */
const QList< QList<int> > CSerialReader::__ParseData(const QStringList & iStringList)
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

/**
 * @brief CSerialReader::OpenSerial
 * 打开对应8963串口,如果已经打开也返回true
 *
 * @return ：打开成功返回true，否则false
 */
bool CSerialReader::OpenSerial(int nProductID)
{
    QList<QSerialPortInfo> iSList = QSerialPortInfo::availablePorts();

    int index = -1;
    for (int i = 0; i < iSList.size(); i++)
    {
        if (nProductID == iSList[i].productIdentifier())
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        qDebug() << "serial port not found";
        return false;
    }


    m_iPort.setPort(iSList[index]);

    if (m_iPort.isOpen())
    {
        qDebug() << "Port already opened";
        return true;
    }

    if (false == m_iPort.open(QSerialPort::ReadWrite))
    {
        qDebug() << "Open serial port failed";
        return false;
    }

    qDebug() << "Open serial port succeed";
    return true;
}

/**
 * @brief CSerialReader::CloseSerial
 * 关闭串口，如果没有打开也返回true
 *
 * @return
 */
void CSerialReader::CloseSerial()
{
    qDebug() << "Close serial port";
    m_iPort.close();
}


/**
 * @brief CSerialReader::ReadSerial
 * 需要先打开串口
 * 读取一次串口数据
 *
 * @return ：读取的数据,QStringList形式保存，每条为一次记录
 */
const QList< QList<int> > CSerialReader::ReadSerial()
{
    QStringList iList;

    char buffer[5000] = {0};

    // 读取前需要设置阻塞
    if (!m_iPort.waitForReadyRead(5000))
    {
        qDebug() << "No data read in 5s";
        return QList< QList<int> >();
    }
    m_iPort.read(buffer, 5000);

    // 提取完整数据，需要满足以#开头，*结尾，20个数据
    QRegExp rx("(#\\s(\\d{3,4}\\s){20}\\*\r\n)");
    rx.setMinimal(true); // 设置非贪心模式，匹配最小长度

    int pos = 0;
    // rx.indexIn返回的在str中的位置
    while ((pos = rx.indexIn(QString(buffer), pos)) != -1)
    {
        // 往iList中插如第一个括号匹配的内容
        iList << rx.cap(1);
        pos += rx.matchedLength(); // 移动开始匹配模式的位置，避免重复匹配
    }

    return __ParseData(iList);
}

CSerialReader* CSerialReader::getReader()
{
    if (m_pReader == NULL)
        m_pReader = new CSerialReader();
    return m_pReader;
}

