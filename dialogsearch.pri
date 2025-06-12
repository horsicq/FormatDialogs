INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogsearch.h \
    $$PWD/searchprocess.h

SOURCES += \
    $$PWD/dialogsearch.cpp \
    $$PWD/searchprocess.cpp

FORMS += \
    $$PWD/dialogsearch.ui

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/xdialogprocess.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/dialogsearch.cmake
