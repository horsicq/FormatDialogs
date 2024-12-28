include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XLINEEDITHEX_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xlineedithex.cmake)
    set(DIALOGREMOVE_SOURCES ${DIALOGREMOVE_SOURCES} ${XLINEEDITHEX_SOURCES})
endif()

set(DIALOGREMOVE_SOURCES
    ${DIALOGREMOVE_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogremove.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogremove.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogremove.ui
    )
