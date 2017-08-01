#-------------------------------------------------
#
# Project created by QtCreator 2017-07-28T12:39:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tutorial_02_Widget
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


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    audio/music.cpp \
    audio/soundmanager.cpp \
    widgets/singlemusicmixer.cpp \
    widgets/reorderview.cpp

HEADERS += \
    defines.h \
    mainwindow.h \
    audio/music.h \
    audio/soundmanager.h \
    widgets/singlemusicmixer.h \
    widgets/reorderview.h

FORMS += \
        mainwindow.ui \
    widgets/singlemusicmixer.ui \
    test.ui

win32: LIBS += -L$$PWD/libs/PortAudio/releaseMin/ -lportaudio_x64

INCLUDEPATH += $$PWD/libs/PortAudio/include
DEPENDPATH += $$PWD/libs/PortAudio/include

win32: LIBS += -L$$PWD/libs/libsndfile_x64/lib/ -llibsndfile-1

INCLUDEPATH += $$PWD/libs/libsndfile_x64/include
DEPENDPATH += $$PWD/libs/libsndfile_x64/include

win32: LIBS += -L$$PWD/libs/soxr_x64/lib/ -lsoxr

INCLUDEPATH += $$PWD/libs/soxr_x64/include
DEPENDPATH += $$PWD/libs/soxr_x64/include