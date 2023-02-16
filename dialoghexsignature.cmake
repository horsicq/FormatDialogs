include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../FormatWidgets/SearchValues/searchvalueswidget.cmake)

set(DIALOGHEXSIGNATURE_SOURCES
    ${SEARCHVALUESWIDGET_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialoghexsignature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoghexsignature.ui
    )
