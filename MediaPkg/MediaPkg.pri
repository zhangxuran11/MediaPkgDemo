INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
include(ZTools/ZTools.pri)
HEADERS += \
    MediaPkg/sdpsrcparser.h \
    MediaPkg/CQmedia.h \
    MediaPkg/CQipc.h \
    MediaPkg/CQGstbasic.h \
    MediaPkg/MediaPkgCommon.h

SOURCES += \
    MediaPkg/sdpsrcparser.cpp \
    MediaPkg/CQmedia.cpp \
    MediaPkg/CQipc.cpp \
    MediaPkg/CQGstbasic.cpp \
    MediaPkg/MediaPkgCallback.cpp
