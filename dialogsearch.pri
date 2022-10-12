INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogsearch.h \
    $$PWD/dialogsearchprocess.h \
    $$PWD/searchprocess.h

SOURCES += \
    $$PWD/dialogsearch.cpp \
    $$PWD/dialogsearchprocess.cpp \
    $$PWD/searchprocess.cpp

FORMS += \
    $$PWD/dialogsearch.ui \
    $$PWD/dialogsearchprocess.ui

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/xdialogprocess.pri)
}

# mb TODO cmake

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md
