#include "MainWindow.h"
#include <QApplication>


#include "cdatabase.h"
#include "Log.h"
#include "DAO.h"

void testCon()
{
    if(CDatabase::getDB().isOpen())
        return;
    else
        throw "db can not open";
}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   MainWindow w;
   w.show();

    return a.exec();
}


