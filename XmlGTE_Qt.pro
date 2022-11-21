QT       += core widgets network xml xmlpatterns script

TARGET = XmlGTE
TEMPLATE = app

win32:DESTDIR += bin/windows
unix:DESTDIR += bin/linux
UI_DIR += tmp
MOC_DIR += tmp
RCC_DIR += tmp
OBJECTS_DIR += tmp
CONFIG += CONFIG_QTCSSH
CONFIG += c++11
QMAKE_CXXFLAGS += "-fno-sized-deallocation"


#Set Application's Icon
RC_FILE = qrc/GTE.rc

DEPENDPATH += . forms includes qrc sources ../CommonLibsQt5/bin bin
INCLUDEPATH += . tmp includes ../CommonLibsQt5/PteCommon

SOURCES +=  sources/main_XmlGTE_Qt.cpp\
            sources/SelectTestcaseDlg.cpp \
            sources/XmlGTE_Qt.cpp \
            sources/BteViewSortFilterProxyModel.cpp

HEADERS  += includes/SelectTestcaseDlg.h \
            includes/XmlGTE_Qt.h \
            includes/BteViewSortFilterProxyModel.h

FORMS    += forms/SelectTestcaseDlg.ui \
    forms/AboutXmlGTE.ui \
    forms/XmlGTE.ui

OTHER_FILES +=

RESOURCES+= qrc/Rcc.qrc

CONFIG(release, debug|release): LIBS += -L$$PWD/../CommonLibsQt5/bin/ -lPteCommon
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../CommonLibsQt5/bin/ -lPteCommond

win32:LIBS += -lWs2_32

INCLUDEPATH += $$PWD/../CommonLibsQt5/PteCommon
DEPENDPATH += $$PWD/../CommonLibsQt5/PteCommon


win32:commonlibs.path = bin/windows
unix:commonlibs.path = bin/linux

CONFIG_QTCSSH {
    CONFIG(release, debug|release) {
        DEFINES    += QT_NO_DEBUG_OUTPUT
        #release code here
        LIBS += -L$$PWD/../CommonLibsQt5/qt-ssh-libs/ -lQSsh -lbotan-2
        win32:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/QSsh.dll
        unix:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/*QSsh.so*
../CommonLibsQt5/qt-ssh-libs/*botan-2.so*
    }
    else:CONFIG(debug, debug|release){
        #debug code here
        LIBS += -L$$PWD/../CommonLibsQt5/qt-ssh-libs/ -lQSsh -lbotan-2
        win32:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/QSshd.dll
        unix:commonlibs.files = ../CommonLibsQt5/qt-ssh-libs/*QSsh.so*
../CommonLibsQt5/qt-ssh-libs/*botan-2.so*
    }
    INCLUDEPATH += $$PWD/../CommonLibsQt5/QSsh/src/libs/qssh
    DEPENDPATH += $$PWD/../CommonLibsQt5/QSsh/src/libs/qssh
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
