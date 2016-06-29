#ifndef CCAMERAREADER_H
#define CCAMERAREADER_H

#include <QObject>
#include <QTimer>

namespace cv {
    class VideoCapture;
    class Mat;
}
using namespace cv;

///
/// \brief The CCameraReader class
/// is mainly used for preparing data for FaceLogic classification.
/// Opening and closing camera is limited inside the class.
///
///
class CCameraReader : public QObject
{
    Q_OBJECT
public:
    CCameraReader();
    ~CCameraReader();

    static QImage Mat2QImage(const Mat &frame);

    const Mat getFrame();
    void openCamera();
    void closeCamera();


private:
    VideoCapture *mp_camera;
    QTimer *mp_timer;

signals:
    void updateCameraDisp(const QImage &img);

public slots:
    void startDispCamera();
    void stopDispCamera();

private slots:
    void timeOut();

};

#endif // CCAMERAREADER_H
