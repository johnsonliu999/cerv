#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T21:56:41
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cerv
TEMPLATE = app

INCLUDEPATH += /usr/local/include \
/usr/local/include/opencv \
/usr/local/include/opencv2 \


LIBS += /usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_objdetect.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_videoio.so \

FORMS += \
    collectdialog.ui \
    logdialog.ui \
    MainWindow.ui \
    registerdialog.ui \
    ReportWindow.ui

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
    Utils.h

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
    SitLogic.cpp


DISTFILES += \
    README.md
