include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xlineedithex.cmake)

set(DIALOGREMOVE_SOURCES
    ${XLINEEDITHEX_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogremove.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogremove.ui
    )
