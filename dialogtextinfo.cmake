include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XZIP_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XArchive/xzip.cmake)
    set(DIALOGTEXTINFO_SOURCES ${DIALOGTEXTINFO_SOURCES} ${XZIP_SOURCES})
endif()

set(DIALOGTEXTINFO_SOURCES
    ${DIALOGTEXTINFO_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogtextinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogtextinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogtextinfo.ui
    )
