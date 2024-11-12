project(fox5)

set(fox5_SOURCE_FILES
    fox5cipher.cpp
    fox5.cpp
)

set(fox5_HEADER_FILES
    fox5cipher.h
    fox5.h
)

add_library(${PROJECT_NAME} STATIC ${fox5_SOURCE_FILES} ${fox5_HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE lzma)