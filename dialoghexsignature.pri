INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialoghexsignature.ui

HEADERS += \
    $$PWD/dialoghexsignature.h

SOURCES += \
    $$PWD/dialoghexsignature.cpp

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include($$PWD/../XOptions/xoptions.pri)
}
