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

set(CMAKE_C_FLAGS "-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft")
set(CMAKE_EXE_LINKER_FLAGS "-specs=3dsx.specs -Wl,--build-id=none -L/opt/devkitpro/libctru/lib -L/opt/devkitpro/portlibs/3ds/lib -lctru -lm")

add_library(${PROJECT_NAME} STATIC ${lzma_SOURCE_FILES} ${lzma_HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})