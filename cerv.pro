#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T21:56:41
#
#-------------------------------------------------

QT       += core gui sql serialport

#CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cerv
TEMPLATE = app

# ubuntu
#INCLUDEPATH += /usr/local/include \
#/usr/local/include/opencv \
#/usr/local/include/opencv2 \

#LIBS += /usr/local/lib/libopencv_core.so \
#/usr/local/lib/libopencv_ml.so \
#/usr/local/lib/libopencv_highgui.so \
#/usr/local/lib/libopencv_objdetect.so \
#/usr/local/lib/libopencv_imgproc.so \
#/usr/local/lib/libopencv_videoio.so \

FORMS += \
    collectdialog.ui \
    logdialog.ui \
    MainWindow.ui \
    registerdialog.ui \
    ReportWindow.ui \
    connectdialog.ui \
    trainfacewidget.ui

INCLUDEPATH += D:/Download/OpenCV/opencv/build/include \
D:/Download/OpenCV/opencv/build/include/opencv \
D:/Download/OpenCV/opencv/build/include/opencv2

HEADERS += \
    capture.h \
    cdatabase.h \
    collectdialog.h \
    cpredictor.h \
    cserialreader.h \
    DAO.h \
    FaceLogic.h \
    Log.h \
    logdialog.h \
    MainWindow.h \
    MySession.h \
    Predictor.h \
    registerdialog.h \
    ReportWindow.h \
    SitLogic.h \
    TotalLogic.h \
    User.h \
    Utils.h \
    connectdialog.h \
    ccamerareader.h \
    cfaceclassfier.h \
    trainfacewidget.h \
    logger.h \
    loadder.h

SOURCES += \
    capture.cpp \
    cdatabase.cpp \
    collectdialog.cpp \
    cpredictor.cpp \
    cserialreader.cpp \
    DAO.cpp \
    FaceLogic.cpp \
    logdialog.cpp \
    main.cpp \
    MainWindow.cpp \
    MySession.cpp \
    registerdialog.cpp \
    ReportWindow.cpp \
    SitLogic.cpp \
    connectdialog.cpp \
    ccamerareader.cpp \
    cfaceclassfier.cpp \
    trainfacewidget.cpp \
    logger.cpp \
    loadder.cpp


DISTFILES += \
    README.md

RC_ICONS += logo48.ico






# windows
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Download/OpenCV/opencv/build/x64/vc14/lib/ -lopencv_world310
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Download/OpenCV/opencv/build/x64/vc14/lib/ -lopencv_world310d
else:unix: LIBS += -L$$PWD/../../../Download/OpenCV/opencv/build/x64/vc14/lib/ -lopencv_world310

INCLUDEPATH += $$PWD/../../../Download/OpenCV/opencv/build/x64/vc14
DEPENDPATH += $$PWD/../../../Download/OpenCV/opencv/build/x64/vc14
