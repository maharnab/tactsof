#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T16:44:04
#
#-------------------------------------------------

QT       += core gui
QT       += network

include(../GUI_01/src/qextserialport.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = GUI_01
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    datadisplay.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    datadisplay.h \
    about.h

FORMS    += mainwindow.ui \
    datadisplay.ui \
    about.ui

RESOURCES += \
    resource.qrc \
    resource.qrc
