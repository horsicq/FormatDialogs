include_directories(${CMAKE_CURRENT_LIST_DIR})

set(DIALOGSEARCH_SOURCES
    ${DIALOGSEARCH_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogsearch.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogsearch.h
    ${CMAKE_CURRENT_LIST_DIR}/searchprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/searchprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogsearch.ui
)
