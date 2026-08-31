INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogtextinfo.h

SOURCES += \
    $$PWD/dialogtextinfo.cpp

FORMS += \
    $$PWD/dialogtextinfo.ui

!contains(XCONFIG, xzip) {
    XCONFIG += xzip
    include($$PWD/../XArchive/xzip.pri)
}
