include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XSHORTCUTS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XShortcuts/xshortcuts.cmake)
    set(XDIALOGPROCESS_SOURCES ${XDIALOGPROCESS_SOURCES} ${XSHORTCUTS_SOURCES})
endif()

set(XDIALOGPROCESS_SOURCES
    ${XDIALOGPROCESS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xdialogprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdialogprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/xdialogprocess.ui
    )
