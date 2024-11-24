project(fox5)

set(fox5_SOURCE_FILES
    fox5.cpp
)

set(fox5_HEADER_FILES
    fox5palette.h
    fox5.h
)

# Conditionally add fox5cipher.cpp if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fox5cipher.cpp")
    list(APPEND fox5_SOURCE_FILES fox5cipher.cpp)
endif()

# Conditionally add fox5cipher.h if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fox5cipher.h")
    list(APPEND fox5_HEADER_FILES fox5cipher.h)
endif()

set_source_files_properties(${fox5_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND fox5_SOURCE_FILES ${fox5_HEADER_FILES})

add_library(${PROJECT_NAME} STATIC ${fox5_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE lzma)
