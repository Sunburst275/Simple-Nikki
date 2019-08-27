#-------------------------------------------------
#
# Project created by QtCreator 2019-07-29T17:12:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simpleNikki
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

win32:RC_ICONS += simplenikkiicons.ico

SOURCES += \
        addprofiledialog.cpp \
        closeoperationdialog.cpp \
        errlogerasedialog.cpp \
        errorlogwaseraseddialog.cpp \
        firststartdialog.cpp \
        main.cpp \
        mainwindow.cpp \
        nikkidebug.cpp \
        settingswereeraseddialog.cpp \
        settingswindow.cpp \
        timethread.cpp

HEADERS += \
        addprofiledialog.h \
        closeoperationdialog.h \
        errlogerasedialog.h \
        errorlogwaseraseddialog.h \
        firststartdialog.h \
        mainwindow.h \
        nikkidebug.h \
        profile.h \
        settings.h \
        settingswereeraseddialog.h \
        settingswindow.h \
        timethread.h

FORMS += \
        addprofiledialog.ui \
        closeoperationdialog.ui \
        errlogerasedialog.ui \
        errorlogwaseraseddialog.ui \
        firststartdialog.ui \
        mainwindow.ui \
        settingswereeraseddialog.ui \
        settingswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    nikkiressources.qrc
