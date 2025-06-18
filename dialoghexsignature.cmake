include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED SEARCHVALUESWIDGET_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatWidgets/SearchValues/searchvalueswidget.cmake)
    set(DIALOGHEXSIGNATURE_SOURCES ${DIALOGHEXSIGNATURE_SOURCES} ${SEARCHVALUESWIDGET_SOURCES})
endif()

set(DIALOGHEXSIGNATURE_SOURCES
    ${DIALOGHEXSIGNATURE_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialoghexsignature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoghexsignature.h
    ${CMAKE_CURRENT_LIST_DIR}/dialoghexsignature.ui
    )
