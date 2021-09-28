QT       += core gui widgets network

CONFIG += c++11

include(../optimize.pri)

SOURCES += \
    addshapes.cpp \
    main.cpp \
    mainwindow.cpp \
    mapshapedata.cpp

HEADERS += \
    mainwindow.hpp \
    mapshapedata.hpp

FORMS += \
    mainwindow.ui \
    mapshapedata.ui

RESOURCES += \
  84.qrc \
  icons.qrc

LIBNAME = town
include(../pretargetdeps.pri)
include(../libneeded.pri)
LIBNAME = mapctrl
include(../pretargetdeps.pri)
include(../libneeded.pri)
LIBNAME = tools
include(../pretargetdeps.pri)
include(../libneeded.pri)

include(postlink.pri)
