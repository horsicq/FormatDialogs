include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XLINEEDITHEX_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xlineedithex.cmake)
    set(DIALOGRESIZE_SOURCES ${DIALOGRESIZE_SOURCES} ${XLINEEDITHEX_SOURCES})
endif()

set(DIALOGRESIZE_SOURCES
    ${DIALOGRESIZE_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogresize.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogresize.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogresize.ui
    )
