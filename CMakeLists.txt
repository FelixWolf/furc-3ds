cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME MyProject)
# Cross compiling
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(PLATFORM STREQUAL "EMSCRIPTEN")
        execute_process(COMMAND em-config EMSCRIPTEN_ROOT OUTPUT_VARIABLE EMSCRIPTEN_ROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(CMAKE_TOOLCHAIN_FILE "${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake"  CACHE STRING "Toolchain file for Emscripten")
        
    elseif(PLATFORM STREQUAL "PSVITA")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE STRING "Toolchain file for PSVita")
        include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
        
    elseif(PLATFORM STREQUAL "N3DS")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{DEVKITPRO}/cmake/3DS.cmake" CACHE STRING "Toolchain file for 3DS")
        set(N3DS ON CACHE STRING "Target N3DS") #DevkitPro doesn't seem to set a variable like this
    
    elseif(PLATFORM STREQUAL "MINGW64")
        # the name of the target operating system
        set(CMAKE_SYSTEM_NAME Windows)

        # which compilers to use for C and C++
        set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
        set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

        # where is the target environment located
        set(CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32)

        # adjust the default behavior of the FIND_XXX() commands:
        # search programs in the host environment
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

        # search headers and libraries in the target environment
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    
    elseif(PLATFORM STREQUAL "MINGW32")
        # the name of the target operating system
        set(CMAKE_SYSTEM_NAME Windows)

        # which compilers to use for C and C++
        set(CMAKE_C_COMPILER   i686-w64-mingw32-gcc)
        set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)

        # where is the target environment located
        set(CMAKE_FIND_ROOT_PATH  /usr/i686-w64-mingw32)

        # adjust the default behavior of the FIND_XXX() commands:
        # search programs in the host environment
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

        # search headers and libraries in the target environment
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    
    endif ()
endif ()

# We doing a C++ project
project(${PROJECT_NAME} CXX)

# Set architecture based on the target operating system
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITINESS 64)

elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(BITINESS 32)

endif()

if (WIN32)
    set(ARCH "win${BITINESS}")

elseif (EMSCRIPTEN)
    set(ARCH "emscripten")

elseif (UNIX AND NOT APPLE)
    set(ARCH "linux${BITINESS}")

elseif (UNIX AND APPLE)
    set(ARCH "osx${BITINESS}")

elseif (TARGET_WASM)
    set(ARCH "wasm")

elseif (VITA)
    set(ARCH "vita")

elseif (N3DS)
    set(ARCH "3ds")

else ()
    message(FATAL_ERROR "Unsupported operating system")

endif()

# Set output directory for build artifacts
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build-${ARCH})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_custom_target(package_all)

set(PACKAGE_DIR ${CMAKE_BINARY_DIR}/packaged)
# Define the function for packaging files
function(package_files src target)
    string(UUID name NAMESPACE "a5f4f2bb-85a3-5811-33f6-2e74d5cccdb4" NAME "${src}:${target}"
       TYPE SHA1 UPPER)
    add_custom_target(
        "package_file_${name}" ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_DIR}/${target}"
        COMMAND ${CMAKE_COMMAND} -E copy "${src}" "${PACKAGE_DIR}/${target}/"
        COMMENT "${name}: Copying ${src} to /${target}"
    )
    add_dependencies(package_all package_file_${name})
endfunction()
function(package_dir src target)
    string(UUID name NAMESPACE "c2801807-80dc-6ee6-7e03-844a3a55e8f0" NAME "${src}:${target}"
       TYPE SHA1 UPPER)
    add_custom_target(
        package_dir_${name} ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_DIR}/${target}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${src}" "${PACKAGE_DIR}/${target}"
        COMMENT "Packaging directory ${src} to ${PACKAGE_DIR}/${target}"
        COMMENT "${name}: Copying ${src} to /${target}"
    )

    # Make the main target depend on this custom packaging target
    add_dependencies(package_all package_dir_${name})
endfunction()

# Add subdirectories
add_subdirectory(src)

# Specify include directories
include_directories(${CMAKE_SOURCE_DIR}/libs)

# Optionally, configure third-party libraries
# add_subdirectory(libs/library_name)

# Optionally, link libraries
# target_link_libraries(${PROJECT_NAME} library_name)
