#ifndef TRAINFACEWIDGET_H
#define TRAINFACEWIDGET_H

#include <QWidget>
#include <QThread>

#include "cfaceclassfier.h"
#include "ccamerareader.h"


namespace Ui {
class TrainFaceWidget;
}

class TrainFaceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainFaceWidget(QWidget *parent = 0);
    ~TrainFaceWidget();

private:
    Ui::TrainFaceWidget *ui;
    CFaceClassfier* mp_classifier;
    CCameraReader *mp_reader;
    QThread *mp_procThread;

signals:
    void startTrain();
    void startDispCamera();
    void stopDispCamera();

public slots:
    void info(const QString &title, const QString &text);
    void updateCameraDisp(const QImage &img);
    void updateDisp(const QImage& img, const QString& remainTime);
private slots:
    void on_startBtn_clicked();
};

#endif // TRAINFACEWIDGET_H
