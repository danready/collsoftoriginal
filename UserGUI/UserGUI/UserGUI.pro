#-------------------------------------------------
#
# Project created by QtCreator 2016-07-13T13:14:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UserGUI
TEMPLATE = app

CONFIG += c++11

LIBS += -lldap

SOURCES += main.cpp\
        mainwindow.cpp \
    enablepasswordusergui.cpp \
    enablepasswordexpertmode.cpp \
    logfileusergui.cpp \
    commandmaker.cpp \
    generaldialog.cpp \
    doublechecking.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    enablepasswordusergui.h \
    enablepasswordexpertmode.h \
    logfileusergui.h \
    commandmaker.h \
    DefineGeneral.h \
    generaldialog.h \
    doublechecking.h \
    about.h

FORMS    += mainwindow.ui \
    enablepasswordusergui.ui \
    enablepasswordexpertmode.ui \
    generaldialog.ui \
    doublechecking.ui \
    about.ui

RESOURCES += \
    Resource.qrc

DESTDIR = ../bin
OBJECTS_DIR = ../bin/.obj
MOC_DIR = ../bin/.moc
RCC_DIR = ../bin/.rcc
UI_DIR = ../bin/.ui
