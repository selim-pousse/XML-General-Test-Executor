#-------------------------------------------------
#
# Project created by QtCreator 2016-03-19T13:54:56
#
#-------------------------------------------------

QT       += core network xml xmlpatterns script

QT       -= gui

TARGET = xmlgte
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += CONFIG_QTCSSH

TEMPLATE = app

DESTDIR += bin
UI_DIR += tmp
MOC_DIR += tmp
RCC_DIR += tmp
OBJECTS_DIR += tmp

DEPENDPATH += . includes sources ../CommonLibsQt5/bin bin
INCLUDEPATH += . tmp includes ../CommonLibsQt5/PteCommon

SOURCES += sources/main_xmlgte.cpp \
           sources/xmlgte.cpp

HEADERS += \
            includes/xmlgte.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../CommonLibsQt5/bin/ -lPteCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../CommonLibsQt5/bin/ -lPteCommond
else:unix: LIBS += -L$$PWD/../CommonLibsQt5/bin/ -lPteCommon

win32:LIBS += -lWs2_32

INCLUDEPATH += $$PWD/../CommonLibsQt5/PteCommon
DEPENDPATH += $$PWD/../CommonLibsQt5/PteCommon

commonlibs.path = bin
CONFIG_QTCSSH {
    CONFIG(release, debug|release) {
        DEFINES    += QT_NO_DEBUG_OUTPUT
        #release code here
        LIBS += -L$$PWD/../CommonLibsQt5/qt-ssh-libs/ -lQtcSsh4
        win32:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/QtcSsh4.dll
        unix:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/*QtcSsh4.so*
    }
    else:CONFIG(debug, debug|release){
        #debug code here
        LIBS += -L$$PWD/../CommonLibsQt5/qt-ssh-libs/ -lQtcSshd4
        win32:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/QtcSshd4.dll
        unix:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/*QtcSshd4.so*
    }
    INCLUDEPATH += $$PWD/../CommonLibsQt5/qtc-ssh/src/libs/ssh
    DEPENDPATH += $$PWD/../CommonLibsQt5/qtc-ssh/src/libs/ssh
} else {
    CONFIG(release, debug|release) {
        DEFINES    += QT_NO_DEBUG_OUTPUT
        #release code here
        LIBS += -L$$PWD/../CommonLibsQt5/qt-ssh-libs/ -lssh
        win32:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/ssh.dll
        unix:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/*ssh.so*
    }
    else:CONFIG(debug, debug|release){
        #debug code here
        LIBS += -L$$PWD/../CommonLibsQt5/qt-ssh-libs/ -lsshd
        win32:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/sshd.dll
        unix:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/*sshd.so*
    }
    INCLUDEPATH += $$PWD/../CommonLibsQt5/qt-ssh
    DEPENDPATH += $$PWD/../CommonLibsQt5/qt-ssh
}
INSTALLS += commonlibs
