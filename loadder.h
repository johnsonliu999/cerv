#ifndef LOADDER_H
#define LOADDER_H

#include <QObject>

struct Log;

class Loadder : public QObject
{
    Q_OBJECT
public:
    explicit Loadder(QObject *parent = 0);
    ~Loadder();

private:
    QThread *mp_thread;

signals:
    void updateLogTable(const QList<Log> &logList);
    void finishedLoad();
    void info(const QString &title, const QString &text);

public slots:
    void loadLog(const QDate &date);
};

#endif // LOADDER_H
