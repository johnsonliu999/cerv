#ifndef CCAMERAREADER_H
#define CCAMERAREADER_H

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
class CCameraReader
{
public:
    CCameraReader();
    ~CCameraReader();

    const Mat getFrame();
    void openCamera();
    void closeCamera();


private:
    VideoCapture *mp_camera;
};

#endif // CCAMERAREADER_H
