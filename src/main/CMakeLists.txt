# Add the executable target for the main module
project(main LANGUAGES C CXX ASM)

set(main_SOURCE_FILES
    appbase.cpp
    3dstexture.cpp
    3dsshader.cpp
    texturecache.cpp
    sprite.cpp
    scene.cpp
    spritescene.cpp
    demoscene.cpp
    furcadia.cpp
    main.cpp
)

set(main_HEADER_FILES
    appbase.h
    3dstexture.h
    3dsshader.h
    texturecache.h
    singleton.h
    sprite.h
    scene.h
    spritescene.h
    demoscene.h
    furcadia.h
    testimg.h
    main.h
)

set_source_files_properties(${main_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND main_SOURCE_FILES ${main_HEADER_FILES})

set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

include_directories(${GENERATED_DIR})

add_executable(${PROJECT_NAME}
    ${main_SOURCE_FILES}
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if (N3DS)
    target_include_directories(${PROJECT_NAME} PRIVATE $ENV{DEVKITPRO}/libctru/include)
    target_link_libraries(${PROJECT_NAME} PRIVATE citro3d ctru)

    set(main_PICA_FILES
        unlit_generic.v.pica
    )
    

    set(GENERATED_HEADERS)

    file(MAKE_DIRECTORY ${GENERATED_DIR}/shaders)
    foreach(PICA_FILE ${main_PICA_FILES})
        get_filename_component(BASENAME ${PICA_FILE} NAME_WE)
        set(HEADER_FILE ${GENERATED_DIR}/${BASENAME}.shbin.h)
        set(SHBIN_FILE ${GENERATED_DIR}/shaders/${BASENAME}.shbin)
        
        # Add the command to run 'picasso'
        add_custom_command(
            OUTPUT ${HEADER_FILE}
            COMMAND picasso -h ${HEADER_FILE} -o ${SHBIN_FILE} ${PROJECT_SOURCE_DIR}/${PICA_FILE}
            DEPENDS ${PICA_FILE}
            COMMENT "Processing ${PICA_FILE} to generate ${HEADER_FILE}"
        )
        
        # Collect the generated headers
        list(APPEND GENERATED_HEADERS ${HEADER_FILE})
    endforeach()
    add_custom_target(GeneratePicaShaders ALL
        DEPENDS ${GENERATED_HEADERS}
    )
    add_dependencies(${PROJECT_NAME} GeneratePicaShaders)
    dkp_target_generate_symbol_list(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE lzma furcformats)
target_compile_options(${PROJECT_NAME} PRIVATE -Og)

add_custom_target(ElfTo3dsx ALL
    COMMAND 3dsxtool $<TARGET_FILE:main> "${PACKAGE_DIR}/${PROJECT_NAME}.3dsx" --smdh="${PROJECT_SOURCE_DIR}/icon.icn" --romfs="${PACKAGE_DIR}/assets/"
    DEPENDS ${PROJECT_NAME}
)
add_dependencies(ElfTo3dsx GeneratePicaShaders package_all ${PROJECT_NAME})

package_dir(${GENERATED_DIR}/shaders "assets/shaders")
package_dir(${PROJECT_SOURCE_DIR}/assets "assets")

add_dependencies(package_all GeneratePicaShaders ${PROJECT_NAME})
