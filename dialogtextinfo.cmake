include_directories(${CMAKE_CURRENT_LIST_DIR})

# XZIP_SOURCES is the ZIP/decompression core; Formats/xformats.cmake owns it.
# XFORMATS_CORE_ONLY takes just that core and leaves XFORMATS_SOURCES
# undefined, so whichever module composes the full XFormats (xscanengine,
# xfileinfo, ...) still runs its own `if (NOT DEFINED XFORMATS_SOURCES)` fold.
if (NOT DEFINED XZIP_SOURCES)
    set(XFORMATS_CORE_ONLY TRUE)
    include(${CMAKE_CURRENT_LIST_DIR}/../Formats/xformats.cmake)
    unset(XFORMATS_CORE_ONLY)
    set(DIALOGTEXTINFO_SOURCES ${DIALOGTEXTINFO_SOURCES} ${XZIP_SOURCES})
endif()

set(DIALOGTEXTINFO_SOURCES
    ${DIALOGTEXTINFO_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogtextinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogtextinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogtextinfo.ui
    )
