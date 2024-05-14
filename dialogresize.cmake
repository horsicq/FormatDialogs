include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xlineedithex.cmake)

set(DIALOGRESIZE_SOURCES
    ${XLINEEDITHEX_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogresize.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogresize.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogresize.ui
    )
