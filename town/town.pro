QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

SOURCES += \
    town.cpp \
    towns.cpp

HEADERS += \
    town.hpp \
    towns.hpp

include(../optimize.pri)
LIBNAME = tools
include(../pretargetdeps.pri)
