INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogdumpprocess.h \
    $$PWD/dumpprocess.h

SOURCES += \
    $$PWD/dialogdumpprocess.cpp \
    $$PWD/dumpprocess.cpp

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/xdialogprocess.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/dialogdump.cmake
