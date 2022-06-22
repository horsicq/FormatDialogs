include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/xdialogprocess.cmake)

set(DIALOGDUMP_SOURCES
    ${XDIALOGPROCESS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocess.ui
    ${CMAKE_CURRENT_LIST_DIR}/dumpprocess.cpp
    )
