#-------------------------------------------------
#
# Project created by QtCreator 2013-03-07T10:41:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dochazka
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    classday.cpp \
    classmonth.cpp \
    classyear.cpp \
    dialogprehled.cpp

HEADERS  += mainwindow.h \
    classday.h \
    classmonth.h \
    classyear.h \
    dialogprehled.h

FORMS    += mainwindow.ui \
    dialogprehled.ui
