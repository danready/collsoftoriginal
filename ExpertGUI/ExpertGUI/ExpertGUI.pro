#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T09:43:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExpertGUI
TEMPLATE = app

RESOURCES     = file.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    dialog.cpp \
    about.cpp \
    alldriversettingscheck.cpp \
    logfileexpertgui.cpp

HEADERS  += mainwindow.h \
    dialog.h \
    about.h \
    alldriversettingscheck.h \
    logfileexpertgui.h

FORMS    += mainwindow.ui \
    dialog.ui \
    about.ui \
    alldriversettingscheck.ui

CONFIG  += c++11 stl

QMAKE_CXXFLAGS = -std=c++11
LIBS += -pthread

DESTDIR = ../bin
OBJECTS_DIR = ../bin/.obj
MOC_DIR = ../bin/.moc
RCC_DIR = ../bin/.rcc
UI_DIR = ../bin/.ui
