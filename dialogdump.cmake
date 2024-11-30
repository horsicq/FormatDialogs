include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XDIALOGPROCESS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/xdialogprocess.cmake)
    set(DIALOGDUMP_SOURCES ${DIALOGDUMP_SOURCES} ${XDIALOGPROCESS_SOURCES})
endif()

set(DIALOGDUMP_SOURCES
    ${DIALOGDUMP_SOURCES}
    ${XDIALOGPROCESS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogdumpprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/dumpprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dumpprocess.h
    )
