INCLUDEPATH += ../../include/common
WARNINGS += -Wall
TEMPLATE = lib
CONFIG += staticlib
SOURCES += ERMS.cpp PointClasses.cpp
HEADERS += ../../include/common/ERMS.h \
           ../../include/common/PointClasses.h \
           ../../include/common/ANSIICodes.h \
           ../../include/common/PhysicalConstants.h
