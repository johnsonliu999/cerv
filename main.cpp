#include "MainWindow.h"
#include <QApplication>

#include "cserialreader.h"
#include <QSerialPortInfo>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   MainWindow w;
   w.show();

    return a.exec();
}
