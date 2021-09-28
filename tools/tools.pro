CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

#DEFINES += DOUBLECOORDTYPE

include(../optimize.pri)

SOURCES += \
    aabb.cpp \
    ellipsehelper.cpp

HEADERS += \
    aabb.hpp \
    ellipsehelper.hpp \
    kdtree.hpp \
    kdtree_impl.hpp \
    status.hpp \
    tglobals.hpp \
    vector.hpp
