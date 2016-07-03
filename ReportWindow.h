#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include <QMainWindow>
#include <QList>

struct Log;
class QStandardItemModel;
class Loadder;

namespace Ui {
class ReportWindow;
}

class ReportWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit ReportWindow(QWidget *parent, const QDate &date);

    ~ReportWindow();

private:
    Ui::ReportWindow *ui;
    QStandardItemModel *mp_model;
    Loadder *mp_loadder;

    const QDate &m_date;

    void setupTable();

signals:
    void loadLog(const QDate &date);

public slots:
    void updateLogTable(const QList<Log> &logList);
    void info(const QString &title, const QString &text);
    void deleteLoadder();
};

#endif // REPORTWINDOW_H
