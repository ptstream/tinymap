QT -= gui
QT += widgets network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../optimize.pri)

SOURCES += \
    esritileadapter.cpp \
    mapboxtileadapter.cpp \
    mapcircle.cpp \
    mapcross.cpp \
    mapimage.cpp \
    mappolygon.cpp \
    mappolyline.cpp \
    mapshape.cpp \
    maptext.cpp \
    maptooltip.cpp \
    mapwidget.cpp \
    osmtileadapter.cpp \
    tileadapter.cpp

HEADERS += \
    esritileadapter.hpp \
    mapanchoredlocation.hpp \
    mapboxtileadapter.hpp \
    mapcircle.hpp \
    mapcross.hpp \
    mapimage.hpp \
    maplocation.hpp \
    mappolygon.hpp \
    mappolyline.hpp \
    mapshape.hpp \
    maptext.hpp \
    maptooltip.hpp \
    mapwidget.hpp \
    osmtileadapter.hpp \
    tileadapter.hpp

LIBNAME = tools
include(../pretargetdeps.pri)
LIBNAME = town
include(../pretargetdeps.pri)

