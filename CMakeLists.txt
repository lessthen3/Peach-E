####################################### Config Options

option(BUILD_PEACH_CORE_SHARED "Builds peach_core as a shared library" OFF) #build peach_core shared by default uwu
option(BUILD_PEACH_TESTS "build test suite" ON)

####################################### Enforce CMake Version

cmake_minimum_required(VERSION 4.2.0) #need this for vs2026

####################################### Project definition

project(Peach_Engine VERSION 0.0.1 DESCRIPTION "Peach Engine" LANGUAGES CXX C)

####################################### Enforcing C++20 standard

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################################### Enforce C99 Standard

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

####################################### Random Globals

set_property(GLOBAL PROPERTY USE_FOLDERS ON) #dunno where else to put this owo

####################################### Check For No Config Set

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "No build type specified, ending build. CMAKE WILL NOW EXIT")
endif()

####################################### Enforce 64-bit Requirement

if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    message(FATAL_ERROR "Peach-E only supports 64-bit platforms.")
endif()

####################################### Master snuggle list for platform detection UwU (my chatgpt is bricked af)

# Let's set our own little love variable to make CMake oh-so-comfy
set(TARGET_PLATFORM "")

if(WIN32)

    set(TARGET_PLATFORM "windows")
    message(STATUS "Mmm, Windows detected, kitten! 😏")

elseif(APPLE)

    # CMake modern toolchains will set CMAKE_SYSTEM_NAME correctly
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(TARGET_PLATFORM "ios")
        message(STATUS "iOS, purrfect for kitten's paws and paws only~ 🍏🐾")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "tvOS")
        set(TARGET_PLATFORM "tvos")
        message(STATUS "tvOS, time to get cozy on the big screen, nya~ 📺")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "watchOS")
        set(TARGET_PLATFORM "watchos")
        message(STATUS "watchOS, smallest but cuddliest! ⌚️💖")
    else()
        set(TARGET_PLATFORM "macos")
        message(STATUS "macOS, premium fur and silky frameworks for daddy's kitten~ 🍎")
    endif()

elseif(ANDROID)

    set(TARGET_PLATFORM "android")
    message(STATUS "Android detected! Purr in my pocket, take me anywhere~ 🤳🐾")

elseif(UNIX AND NOT APPLE) #apparently cmake lumps apple into the unix bucket which it is kinda technically but not useful here uwu

    # Now we dive even deeper for all the extra *special* UNIXes~
    execute_process(
        COMMAND uname -s
        OUTPUT_VARIABLE OS_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(OS_NAME STREQUAL "Linux")
        set(TARGET_PLATFORM "linux")
        message(STATUS "Nyaa, Linux detected! Flex that Tux, kitten~ 🐧")
    elseif(OS_NAME STREQUAL "FreeBSD")
        set(TARGET_PLATFORM "freebsd")
        message(STATUS "FreeBSD... true BSD-babe detected, ready for that zfs cuddles~ 🦀")
    elseif(OS_NAME STREQUAL "Haiku")
        set(TARGET_PLATFORM "haiku")
        message(STATUS "Haiku OS, lightweight and aesthetic, uwu 💐")
    elseif(OS_NAME MATCHES "PSVita")
        set(TARGET_PLATFORM "psvita")
        message(STATUS "Kitten on the PS Vita, time to go portable, ahh~ 🎮")
    else()
        set(TARGET_PLATFORM "unix")
        message(STATUS "Generic UNIX, so mysterious~ 😽")
    endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "PSVita" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7-vita-eabi")

    set(TARGET_PLATFORM "psvita")
    message(STATUS "PS Vita detected via toolchain! Peach-E on the go, nyan~ 🎮🍑")

else()

    set(TARGET_PLATFORM "unknown")
    message(WARNING "Unknown platform... I’m scared, hold me closer! 🥺")

endif()

# Now daddy will print out where his kitten is running, so you always feel seen and wanted UwU
message(STATUS "TARGET_PLATFORM is set to: ${TARGET_PLATFORM} ~ nya~")

####################################### Find All Source Files

# Peach Core
file(
    GLOB_RECURSE 
    PEACH_CORE_SOURCES 
    CONFIGURE_DEPENDS 
    "src/peach_core/*.cpp" 
    "src/peach_core/*.h"
    "src/peach_core/*.hpp"
)

# Peach Engine
file(
    GLOB_RECURSE 
    PEACH_ENGINE_SOURCES 
    CONFIGURE_DEPENDS 
    "src/peach_engine/main.cpp" #thin wrapper for Peach Engine executable
)

# Peach Editor
file(
    GLOB_RECURSE 
    PEACH_EDITOR_SOURCES 
    CONFIGURE_DEPENDS 
    "src/peach_editor/*.cpp" 
    "src/peach_editor/*.h"
    "src/peach_editor/*.hpp"
)

####################################### Static/Dynamic Library Target

if(BUILD_PEACH_CORE_SHARED)
    set(PEACH_BUILD_TYPE SHARED)
else()
    set(PEACH_BUILD_TYPE STATIC)
endif()

# Create a static or dynamic library for peach_core
add_library(peach_core ${PEACH_BUILD_TYPE} 
    ${PEACH_CORE_SOURCES}
)

target_include_directories(peach_core PUBLIC
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/header_only #header only library includes
    ${PROJECT_SOURCE_DIR}/src/peach_core
)

set_target_properties(peach_core PROPERTIES POSITION_INDEPENDENT_CODE ON) #make peach_core PIC friendly UwU

####################################### peach_core src includes

if(APPLE)
    file(GLOB PHYSFS_SOURCES 
        third_party/peach_core/source_include/physfs/*.c
        third_party/peach_core/source_include/physfs/*.m
    )
else()
    file(GLOB PHYSFS_SOURCES 
        third_party/peach_core/source_include/physfs/*.c
    )
endif()

file(GLOB VKBOOTSTRAP_SOURCES
    third_party/peach_core/source_include/vk-bootstrap/VkBootstrap.cpp
)

file(GLOB VOLK_SOURCES
    third_party/peach_core/source_include/volk/volk.c
)

file(GLOB LUA_SOURCES 
    third_party/peach_core/source_include/lua/*.c
)

file(GLOB LZ4_SOURCES
    third_party/peach_core/source_include/lz4/*.c
)

target_sources(peach_core PRIVATE
    ${PHYSFS_SOURCES}
    ${VKBOOTSTRAP_SOURCES}
    ${VOLK_SOURCES}
    ${LUA_SOURCES}
    ${LZ4_SOURCES}
)

target_include_directories(peach_core PUBLIC
    third_party/peach_core/source_include
)

source_group("third_party/physfs" FILES ${PHYSFS_SOURCES})
source_group("third_party/lua" FILES ${LUA_SOURCES})
source_group("third_party/volk" FILES ${VOLK_SOURCES})
source_group("third_party/vkbootstrap" FILES ${VKBOOTSTRAP_SOURCES})
source_group("third_party/lz4" FILES ${LZ4_SOURCES})


####################################### Executable Targets

if(TARGET_PLATFORM STREQUAL "macos") #create a bundle instead of a regular exe for macOS

    set(MACOSX_BUNDLE TRUE)

    add_executable(Peach_Editor MACOSX_BUNDLE
        ${PEACH_EDITOR_SOURCES}
    )

    # Peach Editor
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        ${PEACH_ENGINE_SOURCES}
    )

    # Paths to MoltenVK runtime stuff
    set(MOLTENVK_LIB "${PROJECT_SOURCE_DIR}/third_party/peach_core/vulkan/steveARM64/libMoltenVK.dylib")
    set(VULKAN_LIB "${PROJECT_SOURCE_DIR}/third_party/peach_core/vulkan/steveARM64/libvulkan.1.dylib")
    set(VALIDATION_LAYER "${PROJECT_SOURCE_DIR}/third_party/peach_core/vulkan/steveARM64/libVkLayer_khronos_validation.dylib")

    set(JSON_CONFIGS "${PROJECT_SOURCE_DIR}/third_party/peach_core/vulkan/steveARM64/vulkan")

    # Where to copy them in the app bundle
    set(APP_CONTENTS_DIR "$<TARGET_BUNDLE_CONTENT_DIR:Peach_Editor>")
    set(MACOS_DIR "${APP_CONTENTS_DIR}/MacOS")
    set(FRAMEWORKS_DIR "${APP_CONTENTS_DIR}/Frameworks")
    set(RESOURCES_DIR "${APP_CONTENTS_DIR}/Resources")

    # Copy the dylibs and ICD JSON into the bundle
    add_custom_command(TARGET Peach_Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FRAMEWORKS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MOLTENVK_LIB}" "${FRAMEWORKS_DIR}/libMoltenVK.dylib"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${VULKAN_LIB}" "${FRAMEWORKS_DIR}/libvulkan.1.dylib"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${VALIDATION_LAYER}" "${FRAMEWORKS_DIR}/libVkLayer_khronos_validation.dylib"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCES_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${JSON_CONFIGS}" "${RESOURCES_DIR}/vulkan"
    )

else()

    # Peach Engine
    add_executable(
        ${PROJECT_NAME} 
        ${PEACH_ENGINE_SOURCES}
    )

    # Peach Editor
    add_executable( 
        Peach_Editor
        ${PEACH_EDITOR_SOURCES}
    )

endif()

# Link Peach Engine to Peach Core
target_link_libraries(${PROJECT_NAME} PRIVATE 
    peach_core
)

# Link Peach_Editor to Peach Core
target_link_libraries(Peach_Editor PRIVATE 
    peach_core
)

target_sources(Peach_Editor PRIVATE
    third_party/peach_editor/tinyxml2/tinyxml2.cpp
)

target_include_directories(Peach_Editor PRIVATE
    third_party/peach_editor/tinyxml2
)

source_group("third_party/tinyxml2" FILES 
    third_party/peach_editor/tinyxml2/tinyxml2.cpp
)

####################################### pseudo source cmake include owo

add_subdirectory(third_party/peach_editor/bc7enc_rdo)

# tell astcenc which ISA to build for based on our platform
if(TARGET_PLATFORM STREQUAL "windows" OR TARGET_PLATFORM STREQUAL "linux")
    set(ASTCENC_ISA_SSE41 ON CACHE BOOL "" FORCE)
elseif(TARGET_PLATFORM STREQUAL "macos")
    set(ASTCENC_UNIVERSAL_BUILD ON CACHE BOOL "" FORCE) # gives u SSE41+AVX2+NEON fat binary
endif()

set(ASTCENC_CLI OFF CACHE BOOL "" FORCE) # dont build the command line tool
set(ASTCENC_SHAREDLIB OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/peach_editor/astcenc)

set_target_properties(bc7enc_rdo PROPERTIES FOLDER "third_party")
set_target_properties(astcenc-sse4.1-static PROPERTIES FOLDER "third_party")

if(MSVC)
    foreach(f_AstcTarget 
        astcenc-sse4.1-static 
        astcenc-avx2-static 
        astcenc-sse2-static
        astcenc-none-static
    )
        if(TARGET ${f_AstcTarget})
            set_property(TARGET ${f_AstcTarget} PROPERTY MSVC_RUNTIME_LIBRARY
                MultiThreaded$<$<CONFIG:Debug>:Debug>
            )
            
            set_target_properties(${f_AstcTarget} PROPERTIES FOLDER "third_party")

        endif()
    endforeach()
endif()

target_link_libraries(Peach_Editor PRIVATE
    bc7enc_rdo
    $<IF:$<STREQUAL:${TARGET_PLATFORM},macos>,astcenc-neon-static,astcenc-sse4.1-static>
)

if(BUILD_PEACH_TESTS)
    file(
        GLOB_RECURSE 
        PEACH_TEST_SOURCES 
        CONFIGURE_DEPENDS 
        "test_suite/*.cpp" 
        "test_suite/*.h"
        "test_suite/*.hpp"
    )

    add_executable( 
        Peach_Tests
        ${PEACH_TEST_SOURCES}
    )

    target_link_libraries(Peach_Tests PRIVATE 
        peach_core
    )

endif()

####################################### Static Imports

# helper macro for reducing repeated boiler plate for vulkan imports uwu
macro(STATIC_IMPORT TARGET RELEASE_LIB DEBUG_LIB INCLUDE_DIR)
    add_library(${TARGET} STATIC IMPORTED)
    set_target_properties(${TARGET} PROPERTIES
        IMPORTED_LOCATION_DEBUG           ${DEBUG_LIB}
        IMPORTED_LOCATION_RELEASE         ${RELEASE_LIB}
        IMPORTED_LOCATION_RELWITHDEBINFO  ${RELEASE_LIB}
        IMPORTED_LOCATION_MINSIZEREL      ${RELEASE_LIB}
        INTERFACE_INCLUDE_DIRECTORIES     ${INCLUDE_DIR}
    )
endmacro()


if(TARGET_PLATFORM STREQUAL "windows")
    # AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"

    ############ GL extensions for legacy systems

    STATIC_IMPORT(glew 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/win64/release/libglew32.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/win64/debug/libglew32d.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/include"
    )

    ############ Crypto

    STATIC_IMPORT(libsodium 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/win64/release/libsodium.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/win64/debug/libsodium.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/include"
    )

    ############ Pretty Fonts

    STATIC_IMPORT(msdfgen_libpng 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/release/libpng16_static.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/debug/libpng16_staticd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_freetype 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/release/freetype.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/debug/freetyped.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_core 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/release/msdfgen-core.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/debug/msdfgen-core.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_ext 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/release/msdfgen-ext.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/win64/debug/msdfgen-ext.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(zlib 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/win64/release/zlibstatic.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/win64/debug/zlibstaticd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/win64/include"
    )

    ############ Geometry Manipulation Operations

    STATIC_IMPORT(clipper2 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/win64/release/Clipper2.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/win64/debug/Clipper2.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/include"
    )

    ############ Jolt Physics 3D

    STATIC_IMPORT(jolt_physics 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/win64/release/Jolt.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/win64/debug/Jolt.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/include"
    )

    # ############ Asset Importing

    STATIC_IMPORT(assimp 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/win64/release_v143/assimp-vc143-mt.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/win64/debug_v143/assimp-vc143-mtd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/include"
    )

    STATIC_IMPORT(assimp_draco 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/win64/release_v143/draco.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/win64/debug_v143/dracod.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/include"
    )

    ############ Windowing and Input

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/win64/release/SDL3-static.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/win64/debug/SDL3-static.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/include"
    )

    ############ 2D physics

    STATIC_IMPORT(box2d 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/win64/release/box2d.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/win64/debug/box2dd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/include"
    )

    ############ dotnet stuff

    STATIC_IMPORT(nethost 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/release/libnethost.lib" #juicy static linkage UwU
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/debug/nethost.lib" #debug dll
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include"
    )

    ############ Vulkan Specific Imports

    STATIC_IMPORT(shaderC 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/shaderc.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/shadercd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/shaderc"
    )

    ############ SPIRV Tools

    STATIC_IMPORT(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPIRV-Tools.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPIRV-Toolsd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPIRV-Tools-diff.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPIRV-Tools-diffd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPIRV-Tools-link.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPIRV-Tools-linkd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPIRV-Tools-lint.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPIRV-Tools-lintd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPIRV-Tools-opt.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPIRV-Tools-optd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPIRV-Tools-reduce.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPIRV-Tools-reduced.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/SPVRemapper.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/SPVRemapperd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    STATIC_IMPORT(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-core.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-cored.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-cpp.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-cppd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-glsl.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-glsld.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-hlsl.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-hlsld.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-msl.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-msld.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-reflect.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-reflectd.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release/spirv-cross-util.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v1.4.313.1/spirv-cross-utild.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

elseif(TARGET_PLATFORM STREQUAL "macos")

    ############ Crypto

    STATIC_IMPORT(libsodium 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/steveARM64/libsodium.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/steveARM64/libsodium.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/include"
    )

    ############ Pretty Fonts

    STATIC_IMPORT(msdfgen_libpng 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libpng16.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libpng16.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_freetype 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libfreetype.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libfreetype.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_core 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libmsdfgen-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libmsdfgen-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_ext 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libmsdfgen-ext.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steveARM64/libmsdfgen-ext.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(zlib 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/steveARM64/libz.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/steveARM64/libz.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/steveARM64/include"
    )

    ############ Geometry Manipulation Operations

    STATIC_IMPORT(clipper2 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/steveARM64/libClipper2.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/steveARM64/libClipper2.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/include"
    )

    ############ Jolt Physics 3D

    STATIC_IMPORT(jolt_physics 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/steveARM64/libJolt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/steveARM64/libJolt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/include"
    )

    # ############ Asset Importing

    STATIC_IMPORT(assimp 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/steveARM64/libassimp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/steveARM64/libassimp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/include"
    )

    STATIC_IMPORT(assimp_draco 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/steveARM64/libdraco.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/steveARM64/libdraco.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/include"
    )

    ############ Windowing and Input

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/steveARM64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/steveARM64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/include"
    )

    ############ 2D physics

    STATIC_IMPORT(box2d 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/steveARM64/libbox2d.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/steveARM64/libbox2d.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/include"
    )

    ############ dotnet stuff

    STATIC_IMPORT(nethost 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/steveARM64/libnethost.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/steveARM64/libnethost.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include"
    )

    ############ Vulkan Specific Imports

    STATIC_IMPORT(shaderC 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/shaderc"
    )

    ############ SPIRV Tools

    STATIC_IMPORT(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    STATIC_IMPORT(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steveARM64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

elseif(TARGET_PLATFORM STREQUAL "linux") #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT
    # AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64"

    ############ GL extensions for legacy systems

    STATIC_IMPORT(glew 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/tux64/libGLEW.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/tux64/libGLEW.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/include"
    )

    ############ Crypto

    STATIC_IMPORT(libsodium 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/tux64/libsodium.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/tux64/libsodium.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/libsodium/include"
    )

    ############ Pretty Fonts

    STATIC_IMPORT(msdfgen_libpng 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libpng16.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libpng16.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_freetype 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libfreetype.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libfreetype.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_core 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libmsdfgen-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libmsdfgen-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(msdfgen_ext 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libmsdfgen-ext.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/tux64/libmsdfgen-ext.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
    )

    STATIC_IMPORT(zlib 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/tux64/libz.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/tux64/libz.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/zlib/tux64/include"
    )

    ############ Geometry Manipulation Operations

    STATIC_IMPORT(clipper2 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/tux64/libClipper2.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/tux64/libClipper2.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/clipper2/include"
    )

    ############ Jolt Physics 3D

    STATIC_IMPORT(jolt_physics 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/tux64/libJolt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/tux64/libJolt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/jolt_physics/include"
    )

    # ############ Asset Importing

    STATIC_IMPORT(assimp 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/tux64/libassimp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/tux64/libassimp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/include"
    )

    STATIC_IMPORT(assimp_draco 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/tux64/libdraco.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/tux64/libdraco.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/assimp/include"
    )

    ############ Windowing and Input

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/tux64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/tux64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/include"
    )

    ############ 2D physics

    STATIC_IMPORT(box2d 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/tux64/libbox2d.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/tux64/libbox2d.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/box2d/include"
    )

    ############ dotnet stuff

    STATIC_IMPORT(nethost 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64/libnethost.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64/libnethost.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include"
    )

    ############ Vulkan Specific Imports

    STATIC_IMPORT(shaderC 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/shaderc"
    )

    ############ SPIRV Tools

    STATIC_IMPORT(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    STATIC_IMPORT(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/tux64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include/spirv_cross"
    )

else()
    message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH_ENGINE IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX SORRY NOT SORRY")

endif()

####################################### Dependency Linking

if(TARGET_PLATFORM STREQUAL "linux")

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        #opengl
        GL
        glew    
    )

elseif(TARGET_PLATFORM STREQUAL "windows")

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        #opengl
        opengl32
        glew    
    )

endif()

# Peach Core
target_link_libraries(peach_core PUBLIC

    #geometry manipulation
    clipper2

    #physics
    jolt_physics #3D
    box2d #2D   

    #cryptography
    libsodium

    #SDL
    SDL3
)

if(TARGET_PLATFORM STREQUAL "macos") #TODO fix iokit and other frameworks that dont exist on ios ipados or tvos and watchos

    target_link_libraries(peach_core PRIVATE
        "-framework AVFoundation"
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreHaptics"
        "-framework CoreVideo"
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        "-framework CoreGraphics"
        "-framework Foundation"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GameController"
        "-framework CoreMedia"
        "-framework ForceFeedback"
        "-framework Carbon" #wtf sdl3 u still use this garbo
        "-framework UniformTypeIdentifiers"
    ) #needed for statically linking SDL3

elseif(WIN32)
    target_link_libraries(peach_core PRIVATE  
        setupapi 
        version 
        imm32 
        winmm 
        kernel32

        $<$<CONFIG:Debug>:msvcrtd> #needed for MTd builds using box2d, physfs and SDL
    ) #needed for statically linking SDL3

    target_link_libraries(Peach_Editor PRIVATE
        #dont change these, the build breaks for debug otherwise
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:msvcrt>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:advapi32>
    ) #needed for statically linking nethost for peach_editor
endif()

# Peach Editor
target_link_libraries(Peach_Editor PUBLIC
    #Tools for compiling, and debuggin SPIRV, not needed for game runtime uwu
    #spirv-tools
    SPIRV_Tools
    SPIRV_Tools_Diff
    SPIRV_Tools_Link
    SPIRV_Tools_Lint
    SPIRV_Tools_Opt
    SPIRV_Tools_Reduce
    SPIRV_Tools_Remapper

    #spirv-cross
    SPIRV_Cross_Core
    SPIRV_Cross_CPP
    SPIRV_Cross_GLSL
    SPIRV_Cross_HLSL
    SPIRV_Cross_MSL
    SPIRV_Cross_Reflect
    SPIRV_Cross_Util
    
    #compilers
    shaderC
    #dotnet
    nethost #used for utilizing the host system's dotnet if it satisfies >=v6.0

    #asset importing, we'll just write an importer for the internal assimp format for modders since assimp isnt fully portable across all targetted platforms
    assimp
    assimp_draco

    #font stuff
    msdfgen_ext
    msdfgen_core
    msdfgen_freetype
    msdfgen_libpng
    zlib 
)

####################################### Compiler Options and Definitions

target_compile_definitions(peach_core PRIVATE 
    $<$<CONFIG:Debug>:PEACH_DEBUG>
    VK_NO_PROTOTYPES # IMPORTANT: tells volk to not use the default vulkan loader
)

target_compile_definitions(Peach_Editor PRIVATE
    $<$<CONFIG:Debug>:PEACH_DEBUG>
)

if(BUILD_PEACH_CORE_SHARED)
    target_compile_definitions(peach_core PRIVATE
        PEACH_BUILD_DYNAMIC
    ) 
endif()

if(WIN32)
    target_compile_definitions(Peach_Editor PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:NETHOST_USE_AS_STATIC> #need this for nethost to be used statically
    )
    target_compile_definitions(peach_core PUBLIC
        SODIUM_STATIC #needed for statically linking libsodium
        GLEW_STATIC #needed for statically linking glew

        PEACH_API_STATIC #used for compiling peach_api as a static lib uwu
    )

    if(MSVC)
        target_compile_options(peach_core PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 
        target_compile_options(Peach_Editor PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 
        target_compile_options(Peach_Tests PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 

        target_link_options(peach_core PRIVATE /NODEFAULTLIB:LIBCMTD)
        target_link_options(Peach_Editor PRIVATE /NODEFAULTLIB:LIBCMTD)
        target_link_options(Peach_Engine PRIVATE /NODEFAULTLIB:LIBCMTD)
        target_link_options(Peach_Tests PRIVATE /NODEFAULTLIB:LIBCMTD)
    endif()

endif()

# Add Compiler Warnings for Clang/GCC (assuming those are the only two ur using on unix like systems uwu)
if (UNIX OR APPLE)
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

####################################### Set Startup Project (Visual Studio & Xcode)

# Set Peach_Editor as the default scheme in Xcode

if(CMAKE_GENERATOR MATCHES "Xcode")
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
    set(CMAKE_XCODE_SCHEME_EXECUTABLE Peach_Editor)
    message(STATUS "Setting Peach_Editor as the default startup project for Xcode")
endif()

# Set Peach_Editor as the startup project for Visual Studio

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Peach_Editor)
    message(STATUS "Setting Peach_Editor as the default startup project for VS2022")
    
    set(NETHOST_DLL_PATH "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/debug>")

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_ENVIRONMENT 
        "$<$<CONFIG:Debug>:PATH=${NETHOST_DLL_PATH};%PATH%>"
    )

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "")
endif()
