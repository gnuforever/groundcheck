#-------------------------------------------------
#
# Project created by QtCreator 2017-10-27T12:12:15
#
#-------------------------------------------------

QT       += core gui
QT       += network
RC_ICONS += g1.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GroundCheck
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += C:/Opencv/include \
               C:/Opencv/include/opencv \
               C:/Opencv/include/opencv2

LIBS += C:/Opencv/x86/mingw/lib/libopencv_*

SOURCES += main.cpp\
        mainwindow.cpp \
    networkthread.cpp

HEADERS  += mainwindow.h \
    comm_def.h \
    networkthread.h

FORMS    += mainwindow.ui
