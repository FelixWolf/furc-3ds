project(lzma)

set(lzma_SOURCE_FILES
    LzmaDec.c
)

set(lzma_HEADER_FILES
    7zTypes.h
    LzmaDec.h
    Compiler.h
    Precomp.h
)

add_library(${PROJECT_NAME} STATIC ${lzma_SOURCE_FILES} ${lzma_HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})