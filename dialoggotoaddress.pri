INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialoggotoaddress.h

SOURCES += \
    $$PWD/dialoggotoaddress.cpp

FORMS += \
    $$PWD/dialoggotoaddress.ui

!contains(XCONFIG, xlineedithex) {
    XCONFIG += xlineedithex
    include($$PWD/../Controls/xlineedithex.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/dialoggotoaddress.cmake
