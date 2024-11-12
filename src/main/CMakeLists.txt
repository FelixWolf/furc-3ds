# Add the executable target for the main module
project(main)

set(main_SOURCE_FILES
    main.cpp
)

set(main_HEADER_FILES
    main.h
)


set_source_files_properties(${main_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND main_SOURCE_FILES ${main_HEADER_FILES})

add_executable(${PROJECT_NAME}
    ${main_SOURCE_FILES}
)

if (N3DS)
    target_include_directories(${PROJECT_NAME} PRIVATE $ENV{DEVKITPRO}/libctru/include)
    target_link_libraries(${PROJECT_NAME} PRIVATE citro3d ctru)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE lzma)

#package_files($<TARGET_FILE:main> "")
#package_files(${PROJECT_SOURCE_DIR}/icon.icn "")
#package_files(${PROJECT_SOURCE_DIR}/banner.bnr "")

add_dependencies(package_all ${PROJECT_NAME})
