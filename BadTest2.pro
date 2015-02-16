#-------------------------------------------------
#
# Project created by QtCreator 2014-12-15T19:22:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BadTest2
TEMPLATE = app


SOURCES += main.cpp\
        badtest2.cpp

HEADERS  += badtest2.h

FORMS    += badtest2.ui

LIBS += -L$$PWD/ -lbass
INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

RESOURCES += \
    apple.qrc
