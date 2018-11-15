#-------------------------------------------------
#
# Project developed by Nintersoft team
# Developer: Mauro Mascarenhas de Araújo
# Contact: mauro.mascarenhas@nintersoft.com
# License: Nintersoft Open Source Code Licence
# Date: 15 of November of 2018
# Version: 2.0.4.1
#
#-------------------------------------------------

QT       += gui network sql

TARGET = DBManager
TEMPLATE = lib

win32 {
    VERSION = 2.0.4.1

    QMAKE_TARGET_COMPANY = Nintersoft
    QMAKE_TARGET_PRODUCT = DBManager
    QMAKE_TARGET_DESCRIPTION = DBManager
    QMAKE_TARGET_COPYRIGHT = Copyright (c) 2018 - Nintersoft

    CONFIG += skip_target_version_ext
}
else {
    VERSION = 2.0.4
    CONFIG += unversioned_libname
}

DEFINES += DBMANAGER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        dbmanager.cpp

HEADERS += \
        dbmanager.h \
        dbmanager_global.h 

unix {
    target.path = /usr/lib
    INSTALLS += target
}
