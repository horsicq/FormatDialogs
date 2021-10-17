INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialogtextinfo.ui

HEADERS += \
    $$PWD/dialogtextinfo.h

SOURCES += \
    $$PWD/dialogtextinfo.cpp

contains(XCONFIG, use_archive) {
    DEFINES += USE_ARCHIVE
    !contains(XCONFIG, xarchives) {
        XCONFIG += xarchives
        include($$PWD/../XArchive/xarchives.pri)
    }
}
