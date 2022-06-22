INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialogsearch.ui \
    $$PWD/dialogsearchprocess.ui

HEADERS += \
    $$PWD/dialogsearch.h \
    $$PWD/dialogsearchprocess.h \
    $$PWD/searchprocess.h

SOURCES += \
    $$PWD/dialogsearch.cpp \
    $$PWD/dialogsearchprocess.cpp \
    $$PWD/searchprocess.cpp

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/xdialogprocess.pri)
}
