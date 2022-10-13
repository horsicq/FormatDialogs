INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogtextinfo.h

SOURCES += \
    $$PWD/dialogtextinfo.cpp

FORMS += \
    $$PWD/dialogtextinfo.ui

contains(XCONFIG, use_archive) {
    DEFINES += USE_ARCHIVE
    !contains(XCONFIG, xarchives) {
        XCONFIG += xarchives
        include($$PWD/../XArchive/xarchives.pri)
    }
}
