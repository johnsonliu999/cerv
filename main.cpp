#include "MainWindow.h"
#include <QApplication>
#include "logdialog.h"

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
    LogDialog l;
    l.show();
    if (l.exec() == QDialog::Accepted)
    {
        MainWindow w;
        w.show();
        return a.exec();
    }

<<<<<<< HEAD
   MainWindow w;
   w.show();

    return a.exec();
=======
    return 0;


>>>>>>> mytest
}


