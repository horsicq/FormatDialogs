include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XLINEEDITHEX_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xlineedithex.cmake)
    set(DIALOGGOTOADDRESS_SOURCES ${DIALOGGOTOADDRESS_SOURCES} ${XLINEEDITHEX_SOURCES})
endif()

set(DIALOGGOTOADDRESS_SOURCES
    ${DIALOGGOTOADDRESS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialoggotoaddress.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialoggotoaddress.h
    ${CMAKE_CURRENT_LIST_DIR}/dialoggotoaddress.ui
    )
