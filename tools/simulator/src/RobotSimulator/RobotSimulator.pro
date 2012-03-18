#-------------------------------------------------
#
# Project created by QtCreator 2012-03-18T14:59:31
#
#-------------------------------------------------

QT       += core gui

TARGET = RobotSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    RobotInterface.cpp \
    GyroSimulator.cpp \
    RobotSimulator.cpp \
    RobotGraphicsItem.cpp \
    MotorSimulator.cpp

HEADERS  += mainwindow.h \
    RobotInterface.h \
    GyroSimulator.h \
    RobotSimulator.h \
    RobotGraphicsItem.h \
    MotorSimulator.h

FORMS    += mainwindow.ui
