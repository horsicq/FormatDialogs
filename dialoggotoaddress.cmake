# TODO guard
include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xlineedithex.cmake)

set(DIALOGGOTOADDRESS_SOURCES
    ${XLINEEDITHEX_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialoggotoaddress.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialoggotoaddress.cpp
    )
