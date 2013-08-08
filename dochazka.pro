#-------------------------------------------------
#
# Project created by QtCreator 2013-03-07T10:41:11
#
#-------------------------------------------------

QT       += core gui printsupport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dochazka
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    classday.cpp \
    classmonth.cpp \
    classyear.cpp \
    dialogprehled.cpp \
    widgettoolbar.cpp \
    mojetable.cpp

HEADERS  += mainwindow.h \
    classday.h \
    classmonth.h \
    classyear.h \
    dialogprehled.h \
    widgettoolbar.h \
    mojetable.h

FORMS    += mainwindow.ui \
    dialogprehled.ui \
    widgettoolbar.ui

unix:!macx: LIBS += -lxl

win32 {
 INCLUDEPATH += 3rdParty/include_cpp
 LIBS += -L$$PWD/3rdParty/32 -llibxl
}

RESOURCES += \
    zdroje.qrc
