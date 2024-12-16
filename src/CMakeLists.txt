# Add source files for the main module
add_subdirectory(lzma)
# Conditionally add the proprietary submodule if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/proprietary/CMakeLists.txt")
    message(STATUS "Proprietary submodule found. Encryption will work.")
    add_subdirectory(proprietary)
else()
    message(STATUS "Proprietary submodule not found. Encryption WILL NOT WORK.")
endif()
add_subdirectory(fox5)
add_subdirectory(main)