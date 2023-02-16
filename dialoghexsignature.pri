INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialoghexsignature.h

SOURCES += \
    $$PWD/dialoghexsignature.cpp

FORMS += \
    $$PWD/dialoghexsignature.ui

!contains(XCONFIG, searchvalueswidget) {
    XCONFIG += searchvalueswidget
    include($$PWD/../FormatWidgets/SearchValues/searchvalueswidget.pri)
}

DISTFILES += \
    $$PWD/dialoghexsignature.cmake

