#-------------------------------------------------
#
# Проект TLogger 2017-01-27T21:29:46
#
#-------------------------------------------------

QT       -= gui

PROJECT = TLogger

CONFIG += create_prl
CONFIG += no_install_prl
CONFIG += create_pc
CONFIG += c++11

TARGET = $$PROJECT
TEMPLATE = lib
VERSION  = 0.0.1

DEFINES += TLOGGER_LIBRARY

SOURCES += $$PWD/src/tlogger.cpp \
           $$PWD/src/inireader.cpp \
           $$PWD/src/ini.cpp

HEADERS += $$PWD/src/tlogger.h \
           $$PWD/src/tlogger_global.h \
           $$PWD/src/inireader.h \
           $$PWD/src/ini.h \
           $$PWD/src/TLogger

DESTDIR = $$PWD/build/lib
MOC_DIR = $$PWD/build/moc
OBJECTS_DIR = $$PWD/build/obj

isEmpty(PREFIX) {
    PREFIX=$$PWD
}

unix {
    target.path = $$PREFIX/lib

    # includes
    includes.path = $$PREFIX/include/$$PROJECT
    includes.files += $$PWD/src/tlogger.h
    includes.files += $$PWD/src/inireader.h
    includes.files += $$PWD/src/tlogger_global.h
    includes.files += $$PWD/src/TLogger

    INSTALLS += target includes

    # pc-file params
    QMAKE_PKGCONFIG_NAME          = $$PROJECT
    QMAKE_PKGCONFIG_FILE          = $$PROJECT
    QMAKE_PKGCONFIG_DESCRIPTION   = The $$PROJECT library
    QMAKE_PKGCONFIG_LIBDIR        = $$target.path
    QMAKE_PKGCONFIG_INCDIR        = $$PREFIX/include
    QMAKE_PKGCONFIG_REQUIRES      =
    QMAKE_PKGCONFIG_DESTDIR       = ../share/pkgconfig #путь берётся относительно установки библиотеки
}
