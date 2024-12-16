project(fox5)

set(fox5_SOURCE_FILES
    fox5.cpp
)

set(fox5_HEADER_FILES
    fox5palette.h
    fox5.h
)

set_source_files_properties(${fox5_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND fox5_SOURCE_FILES ${fox5_HEADER_FILES})

add_library(${PROJECT_NAME} STATIC ${fox5_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE lzma)

if(TARGET proprietary)
    message(STATUS "Linking fox5 with proprietary encryption")
    target_link_libraries(${PROJECT_NAME} PRIVATE proprietary)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_PROPRIETARY)
else()
    message(STATUS "Proprietary encryption not available. Continuing without it.")
endif()
