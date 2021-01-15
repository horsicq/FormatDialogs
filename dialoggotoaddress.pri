INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialoggotoaddress.ui

HEADERS += \
    $$PWD/dialoggotoaddress.h

SOURCES += \
    $$PWD/dialoggotoaddress.cpp

!contains(XCONFIG, xlineedithex) {
    XCONFIG += xlineedithex
    include($$PWD/../Controls/xlineedithex.pri)
}
