#include "MainWindow.h"
#include <QApplication>
#include "logdialog.h"

#include "cdatabase.h"
#include "Log.h"
#include "DAO.h"
#include "connectdialog.h"

#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogDialog l;
    //l.show();
    if (l.exec() == QDialog::Accepted)
    {
        int res = QMessageBox::question(nullptr, "Method", "Wired?", QMessageBox::Yes, QMessageBox::No);
        bool wired = (res == QMessageBox::Yes);

        MainWindow w(wired);
        w.show();
        return a.exec();
    }
    return 0;
}


