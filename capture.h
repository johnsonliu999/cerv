#ifndef CAPTURE
#define CAPTURE


#include <QImage>


void openCamera();


void closeCamera();


QImage detect();


bool isAllowCameraOpen();


void allowCameraOpen();


#endif // CAPTURE

