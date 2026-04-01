####################################### Config Options

option(BUILD_PEACH_CORE_SHARED "Builds peach_core as a shared library" OFF) #build peach_core shared by default uwu

option(BUILD_PEACH_TESTS "build test suite" OFF)
option(BUILD_PEACH_EDITOR "turns on build output for peach editor" ON)
option(BUILD_PEACH_ENGINE "turns on build output for the executable peach engine, doesn't relate to peach_core" ON)

####################################### Enforce CMake Version

cmake_minimum_required(VERSION 4.2.0) #need this for vs2026

####################################### Project definition

project(Peach_Engine VERSION 0.0.1 DESCRIPTION "Peach Engine" LANGUAGES CXX C)

####################################### Enforcing C++20 standard

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################################### Enforce C99 Standard

set(CMAKE_C_STANDARD 17) #needed for box2d owo
set(CMAKE_C_STANDARD_REQUIRED ON)

####################################### Random Globals

set_property(GLOBAL PROPERTY USE_FOLDERS ON) #dunno where else to put this owo

# Add Compiler Warnings for Clang/GCC (assuming those are the only two ur using on unix like systems uwu)
if (PEACH_PLATFORM_IS_UNIX)
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

####################################### Check For No Config Set

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "No build type specified, ending build. CMAKE WILL NOW EXIT")
endif()

####################################### Enforce 64-bit Requirement

if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 8 AND NOT PEACH_VITA)
    message(FATAL_ERROR "Peach-E only supports 64-bit platforms.")
endif()

####################################### Find peach_core source and headers

# Peach Core
file(
    GLOB_RECURSE 
    PEACH_CORE_SOURCES 
    CONFIGURE_DEPENDS 
    "src/peach_core/*.cpp" 
    "src/peach_core/*.h"
    "src/peach_core/*.hpp"
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
)

set_target_properties(peach_core PROPERTIES POSITION_INDEPENDENT_CODE ON) #make peach_core PIC friendly UwU

####################################### peach_core src includes

file(GLOB LUA_SOURCES 
    third_party/peach_core/source_include/lua/*.c
)

file(GLOB LZ4_SOURCES
    third_party/peach_core/source_include/lz4/*.c
)

file(GLOB CLIPPER2_SOURCES
    third_party/peach_core/source_include/clipper2/*.cpp
)

file(GLOB BOX2D_SOURCES
    third_party/peach_core/source_include/box2d/*.c
)

file(GLOB_RECURSE JOLT_SOURCES
    third_party/peach_core/source_include/Jolt/*.cpp #need caps since jolt does the target include thing in its cmake owo
)

file(GLOB FMT_SOURCES
    third_party/peach_core/source_include/fmt/format.cc
    third_party/peach_core/source_include/fmt/os.cc
)

file(GLOB MONOCYPHER_SOURCES
    third_party/peach_core/source_include/monocypher/*.c
)

if(PEACH_PLATFORM_IS_APPLE)
    file(GLOB PHYSFS_SOURCES 
        third_party/peach_core/source_include/physfs/*.c
        third_party/peach_core/source_include/physfs/*.m
    )
else()
    file(GLOB PHYSFS_SOURCES 
        third_party/peach_core/source_include/physfs/*.c
    )
endif(PEACH_PLATFORM_IS_APPLE)

if(PEACH_WINDOWS OR PEACH_LINUX OR PEACH_BSD OR PEACH_HAIKU OR PEACH_ANDROID OR PEACH_WINDOWS_ARM64)

    file(GLOB VOLK_SOURCES
        third_party/peach_core/plat_dep_source_include/volk/volk.c
    )

    file(GLOB VKBOOTSTRAP_SOURCES
        third_party/peach_core/plat_dep_source_include/vk-bootstrap/VkBootstrap.cpp
    )

    target_sources(peach_core PRIVATE
        ${VKBOOTSTRAP_SOURCES}
        ${VOLK_SOURCES}
    )

    source_group("third_party/volk" FILES ${VOLK_SOURCES})
    source_group("third_party/vkbootstrap" FILES ${VKBOOTSTRAP_SOURCES})

    target_include_directories(peach_core PUBLIC
        third_party/peach_core/plat_dep_source_include #include since it passed the plat test
    )
    
endif(PEACH_WINDOWS OR PEACH_LINUX OR PEACH_BSD OR PEACH_HAIKU OR PEACH_ANDROID OR PEACH_WINDOWS_ARM64)

target_sources(peach_core PRIVATE
    ${PHYSFS_SOURCES}
    ${LUA_SOURCES}
    ${LZ4_SOURCES}
    ${CLIPPER2_SOURCES}
    ${BOX2D_SOURCES}
    ${JOLT_SOURCES}
    ${FMT_SOURCES}
    ${MONOCYPHER_SOURCES}
)

if(PEACH_PLATFORM_IS_APPLE)
    target_sources(peach_core PRIVATE src/peach_core/Rendering/Metal/MetalBridge.mm ) #just to get the objc file compiled for the extern "C" call owo
endif()

target_include_directories(peach_core PUBLIC
    third_party/peach_core/source_include #supported across the board owo ily guys ~~nyah
)

source_group("third_party/physfs" FILES ${PHYSFS_SOURCES})
source_group("third_party/lua" FILES ${LUA_SOURCES})
source_group("third_party/lz4" FILES ${LZ4_SOURCES})
source_group("third_party/clipper2" FILES ${CLIPPER2_SOURCES})
source_group("third_party/box2d" FILES ${BOX2D_SOURCES})
source_group("third_party/Jolt" FILES ${JOLT_SOURCES})
source_group("third_party/fmt" FILES ${FMT_SOURCES})
source_group("third_party/monocypher" FILES ${MONOCYPHER_SOURCES})

####################################### helper macro for reducing repeated boiler plate for static linking uwu

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

####################################### Static Linkage

if(PEACH_WINDOWS)
    # AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"

    ############ GL extensions for legacy systems

    STATIC_IMPORT(glew 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/win64/release/libglew32.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/win64/debug/libglew32d.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/include"
    )

    ############ Windowing and Input

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/win64/release/SDL3-static.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/win64/debug/SDL3-static.lib"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/include"
    )

elseif(PEACH_MACOS)

    ############ Windowing and Input

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/steve64/release/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/steve64/debug/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/include"
    )

elseif(PEACH_LINUX) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT

    ############ GL extensions for legacy systems

    STATIC_IMPORT(glew 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/tux64/libGLEW.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/tux64/libGLEW.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/include"
    )

    ############ Windowing and Input

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/tux64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/tux64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL3/include"
    )

else()
    message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH_ENGINE IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX SORRY NOT SORRY")
endif()

####################################### Dependency Linking

if(PEACH_LINUX)

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        #opengl
        GL
        glew    
    )

elseif(PEACH_PLATFORM_IS_WINDOWS)

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        #opengl
        opengl32
        glew    

        setupapi 
        version 
        imm32 
        winmm 
        kernel32
    )

elseif(PEACH_MACOS) #TODO fix iokit and other frameworks that dont exist on ios ipados or tvos and watchos

    target_link_libraries(peach_core PUBLIC
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
        "-lpthread"
    ) #needed for statically linking SDL3

elseif(PEACH_BSD)

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        GL
        glew
    )

    # FreeBSD uses clang by default, pthread is separate
    target_link_libraries(peach_core PUBLIC 
        pthread
    )

elseif(PEACH_HAIKU)    

    find_package(OpenGL REQUIRED)

    # Haiku has its own GL implementation
    target_link_libraries(peach_core PUBLIC
        GL
        be      # Haiku's core API, SDL3 needs this
        network # Haiku network stack
    )

endif()

# Peach Core
target_link_libraries(peach_core PUBLIC
    #SDL
    SDL3
)

####################################### Master snuggle list for platform detection UwU (my chatgpt is bricked af)
# daddy guarantees exactly ONE of these is always defined per build so no ambiguity 🍑
# daddy guaranteed so ig it works OwO

if(PEACH_WINDOWS)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_WINDOWS
        PEACH_PLATFORM_DESKTOP
        PEACH_ARCH_X64
    )

elseif(PEACH_WINDOWS_ARM64)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_WINDOWS
        PEACH_PLATFORM_DESKTOP
        PEACH_ARCH_ARM64
    )

elseif(PEACH_MACOS)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_MACOS
        PEACH_PLATFORM_APPLE
        PEACH_PLATFORM_DESKTOP
        PEACH_ARCH_ARM64 # universal binary covers both but arm64 is primary
    )

elseif(PEACH_IOS)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_IOS
        PEACH_PLATFORM_APPLE
        PEACH_PLATFORM_MOBILE
        PEACH_ARCH_ARM64
    )

elseif(PEACH_TVOS)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_TVOS
        PEACH_PLATFORM_APPLE
        PEACH_ARCH_ARM64
    )

elseif(PEACH_LINUX)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_LINUX
        PEACH_PLATFORM_DESKTOP
        PEACH_ARCH_X64
    )

elseif(PEACH_BSD)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_BSD
        PEACH_PLATFORM_FREEBSD
        PEACH_PLATFORM_DESKTOP
        PEACH_ARCH_X64
    )

elseif(PEACH_HAIKU)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_HAIKU
        PEACH_PLATFORM_DESKTOP
        PEACH_ARCH_X64
    )

elseif(PEACH_ANDROID)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_ANDROID
        PEACH_PLATFORM_MOBILE
        PEACH_ARCH_ARM64
    )

elseif(PEACH_WASM)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_WASM
        PEACH_PLATFORM_WEB
        PEACH_ARCH_WASM32
    )

elseif(PEACH_VITA)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_VITA
        PEACH_PLATFORM_HANDHELD
        PEACH_ARCH_ARMV7
    )

elseif(PEACH_SWITCH)
    target_compile_definitions(peach_core PUBLIC
        PEACH_PLATFORM_SWITCH
        PEACH_PLATFORM_HANDHELD
        PEACH_ARCH_ARM64
    )

endif()

####################################### Renderer Availability Definitions
# gate renderer includes in src based on what makes sense per platform uwu

if(PEACH_WASM)
    target_compile_definitions(peach_core PUBLIC 
        PEACH_RENDERER_WEBGL
    )

elseif(PEACH_VITA)
    target_compile_definitions(peach_core PUBLIC 
        PEACH_RENDERER_OPENGL
    )

elseif(PEACH_SWITCH)
    target_compile_definitions(peach_core PUBLIC 
        PEACH_RENDERER_OPENGL
    )

elseif(PEACH_MACOS OR PEACH_IOS OR PEACH_TVOS)
    target_compile_definitions(peach_core PUBLIC
        PEACH_RENDERER_METAL
    )

elseif(PEACH_WINDOWS OR PEACH_WINDOWS_ARM64 OR PEACH_LINUX OR PEACH_BSD OR PEACH_HAIKU)
    target_compile_definitions(peach_core PUBLIC
        PEACH_RENDERER_VULKAN
        PEACH_RENDERER_OPENGL
    )

elseif(PEACH_ANDROID)
    target_compile_definitions(peach_core PUBLIC
        PEACH_RENDERER_VULKAN
        PEACH_RENDERER_OPENGL # OpenGL ES fallback
    )

endif()

####################################### Compiler Options and Other Definitions

target_compile_definitions(peach_core PUBLIC 
    $<$<CONFIG:Debug>:PEACH_DEBUG>
    SDL_MAIN_HANDLED
)

if(PEACH_WINDOWS OR PEACH_WINDOWS_ARM64 OR PEACH_LINUX OR PEACH_BSD OR PEACH_HAIKU OR PEACH_ANDROID)

    target_compile_definitions(peach_core PUBLIC VK_NO_PROTOTYPES) # IMPORTANT: tells volk to not use the default vulkan loader

endif(PEACH_WINDOWS OR PEACH_WINDOWS_ARM64 OR PEACH_LINUX OR PEACH_BSD OR PEACH_HAIKU OR PEACH_ANDROID)

if(PEACH_VITA)
    target_compile_definitions(peach_core PUBLIC BOX2D_DISABLE_SIMD)
endif(PEACH_VITA)

if(PEACH_PLATFORM_IS_WINDOWS)

    target_compile_definitions(peach_core PUBLIC
        GLEW_STATIC #needed for statically linking glew
        PEACH_API_STATIC #used for compiling peach_api as a static lib uwu
    )

    if(MSVC)
        target_compile_options(peach_core PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 
        
        target_compile_options(peach_core PUBLIC 
            /utf-8
        )
    endif(MSVC)

endif(PEACH_PLATFORM_IS_WINDOWS)

if(BUILD_PEACH_CORE_SHARED)
    target_compile_definitions(peach_core PRIVATE
        PEACH_BUILD_DYNAMIC
    ) 
endif(BUILD_PEACH_CORE_SHARED)

####################################### Executable Targets

if(BUILD_PEACH_EDITOR)

    ####################################### Find Peach_Editor source and headers

    file(
        GLOB_RECURSE 
        PEACH_EDITOR_SOURCES 
        CONFIGURE_DEPENDS 
        "src/peach_editor/*.cpp" 
        "src/peach_editor/*.h"
        "src/peach_editor/*.hpp"
    )

    ####################################### Create executable target

    if(PEACH_MACOS) #create a bundle instead of a regular exe for macOS
        set(MACOSX_BUNDLE TRUE)

        add_executable(Peach_Editor MACOSX_BUNDLE
            ${PEACH_EDITOR_SOURCES}
        )
    else()
        # Peach Editor
        add_executable( 
            Peach_Editor
            ${PEACH_EDITOR_SOURCES}
        )
    endif(PEACH_MACOS)

    ####################################### pseudo source cmake include owo

    file(GLOB TINYXML2_SOURCES
        third_party/peach_editor/source_include/tinyxml2/tinyxml2.cpp
    )

    file(GLOB LODEPNG_SOURCES
        third_party/peach_editor/source_include/lodepng/lodepng.cpp
    )

    file(GLOB SPIRV_CROSS_SOURCES
        third_party/peach_editor/weird_source_include/spirv_cross/*.cpp
    )

    file(GLOB BC7ENC_RDO_SOURCES
        third_party/peach_editor/source_include/bc7enc_rdo/*.cpp
    )

    target_sources(Peach_Editor PRIVATE
        ${TINYXML2_SOURCES}
        ${LODEPNG_SOURCES}
        ${SPIRV_CROSS_SOURCES}
        ${BC7ENC_RDO_SOURCES}
    )

    target_include_directories(Peach_Editor PRIVATE
        third_party/peach_editor/source_include
        third_party/peach_editor/weird_source_include/spirv_cross        # root hpp files
        third_party/peach_editor/weird_source_include/spirv_cross/include # vendored spirv-headers
        ${PROJECT_SOURCE_DIR}/src/peach_core
    )

    source_group("third_party/tinyxml2" FILES ${TINYXML2_SOURCES})
    source_group("third_party/lodepng" FILES ${LODEPNG_SOURCES})
    source_group("third_party/spirv_cross" FILES ${SPIRV_CROSS_SOURCES})
    source_group("third_party/bc7enc_rdo" FILES ${BC7ENC_RDO_SOURCES})

    if(PEACH_MACOS)
        if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
            set(ASTCENC_ISA_NEON  ON  CACHE BOOL "" FORCE)
            set(ASTCENC_ISA_AVX2  OFF CACHE BOOL "" FORCE)
            set(ASTCENC_ISA_SSE41 OFF CACHE BOOL "" FORCE)
            set(ASTCENC_ISA_SSE2  OFF CACHE BOOL "" FORCE)
        else()
            set(ASTCENC_ISA_NEON  OFF CACHE BOOL "" FORCE)
            set(ASTCENC_ISA_AVX2  ON  CACHE BOOL "" FORCE)
            set(ASTCENC_ISA_SSE41 ON  CACHE BOOL "" FORCE)
            set(ASTCENC_ISA_SSE2  ON  CACHE BOOL "" FORCE)
        endif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    endif(PEACH_MACOS)

    # tell astcenc which ISA to build for based on our platform
    if(PEACH_WINDOWS OR PEACH_LINUX)
        set(ASTCENC_ISA_SSE41 ON CACHE BOOL "" FORCE)
    endif()

    set(ASTCENC_CLI OFF CACHE BOOL "" FORCE) # dont build the command line tool
    set(ASTCENC_SHAREDLIB OFF CACHE BOOL "" FORCE)
    set(ASTCENC_DYNAMIC_RUNTIME OFF CACHE BOOL "" FORCE)

    add_subdirectory(third_party/peach_editor/source_include/astcenc)

    if(PEACH_MACOS)
        foreach(f_AstcTarget
            astcenc-neon-static
            astcenc-avx2-static
            astcenc-sse4.1-static
            astcenc-sse2-static
            astcenc-none-static
        )
            if(TARGET ${f_AstcTarget})
                set_target_properties(${f_AstcTarget} PROPERTIES
                    XCODE_ATTRIBUTE_ARCHS       "${CMAKE_OSX_ARCHITECTURES}"
                    XCODE_ATTRIBUTE_VALID_ARCHS "${CMAKE_OSX_ARCHITECTURES}"
                    OSX_ARCHITECTURES           "${CMAKE_OSX_ARCHITECTURES}"
                )
            endif(TARGET ${f_AstcTarget})
        endforeach()

            # on arm64 — exclude x86 ISA targets from the build entirely
    # Xcode will still generate them in the project but won't compile them
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
        foreach(f_AstcTarget
            astcenc-avx2-static
            astcenc-sse4.1-static
            astcenc-sse2-static
        )
            if(TARGET ${f_AstcTarget})
                set_target_properties(${f_AstcTarget} PROPERTIES
                    EXCLUDE_FROM_ALL TRUE
                )
            endif(TARGET ${f_AstcTarget})
        endforeach()
        endif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    endif(PEACH_MACOS)

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
        $<IF:$<STREQUAL:${PEACH_TARGET_PLATFORM},macos>,astcenc-neon-static,astcenc-sse4.1-static>
    )

    ####################################### Import static libs

    if(PEACH_WINDOWS)

        ############ Pretty Fonts

        STATIC_IMPORT(freetype 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/win64/release/freetype.lib"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/win64/debug/freetyped.lib"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/include/freetype2"
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

        ############# Asset Importing

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

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/release/libnethost.lib" #juicy static linkage UwU
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/debug/nethost.lib" #debug dll
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include"
        )

        ############ shaderc

        STATIC_IMPORT(shaderc 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/release_v20260330/shaderc_combined.lib" 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/win64/debug_v20260330/shaderc_combined.lib" 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include"
        )

    elseif(PEACH_MACOS)
        ############ Pretty Fonts

        STATIC_IMPORT(freetype 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/steve64/libfreetype.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/steve64/libfreetyped.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/include/freetype2"
        )

        STATIC_IMPORT(msdfgen_core 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steve64/release/libmsdfgen-core.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steve64/debug/libmsdfgen-core.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
        )

        STATIC_IMPORT(msdfgen_ext 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steve64/release/libmsdfgen-ext.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/steve64/debug/libmsdfgen-ext.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen/include"
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

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/steve64/libnethost_universal.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/steve64/libnethost_universal.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include"
        )

        ############ Vulkan Specific Imports

        STATIC_IMPORT(shaderc 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steve64/libshaderc_combined.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/steve64/libshaderc_combined.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/vulkan/include"
        )

    elseif(PEACH_LINUX) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT
        # AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64"

        ############ Pretty Fonts

        STATIC_IMPORT(freetype 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/win64/release/freetype.lib"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/win64/debug/freetyped.lib"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype/include/freetype2"
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

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include"
        )

        ############ Vulkan Specific Imports

        STATIC_IMPORT(shaderc 
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

    else()
        message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH_ENGINE IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX SORRY NOT SORRY")
    endif()

    ####################################### Compiler Options and Definitions

    if(PEACH_WINDOWS)
        target_compile_definitions(Peach_Editor PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:NETHOST_USE_AS_STATIC> #need this for nethost to be used statically
        )

        if(MSVC)
            target_compile_options(Peach_Editor PRIVATE
                $<$<CONFIG:Debug>:/MTd>
                $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
            ) 

            target_link_options(Peach_Editor PRIVATE 
                $<$<NOT:$<CONFIG:Debug>>:/LTCG> #because nethost was compiled w /GL w/e ig its gonna happen anyways owo
            )
        endif()

    endif()

    ####################################### Linking deps + peach_core

    if(NOT PEACH_WINDOWS AND NOT PEACH_MACOS) #shaderc_combined handles this for windows ^w^
        target_link_libraries(Peach_Editor PRIVATE
            #Tools for compiling, and debuggin SPIRV, not needed for game runtime uwu
            #spirv-tools
            SPIRV_Tools
            SPIRV_Tools_Diff
            SPIRV_Tools_Link
            SPIRV_Tools_Lint
            SPIRV_Tools_Opt
            SPIRV_Tools_Reduce
            SPIRV_Tools_Remapper
        )
    endif(NOT PEACH_WINDOWS AND NOT PEACH_MACOS)

    if(PEACH_PLATFORM_IS_WINDOWS) #needed for statically linking nethost for peach_editor
        target_link_libraries(Peach_Editor PRIVATE 
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:advapi32>
        ) 
    endif(PEACH_PLATFORM_IS_WINDOWS)

    target_link_libraries(Peach_Editor PRIVATE
        #>O<
        peach_core

        #dotnet
        nethost #used for utilizing the host system's dotnet if it satisfies >=v6.0

        #asset importing, we'll just write an importer for the internal assimp format for modders since assimp isnt fully portable across all targetted platforms
        assimp
        assimp_draco

        #font stuff
        msdfgen_ext
        msdfgen_core
        freetype

        #vulkan owo
        shaderc
    )

endif(BUILD_PEACH_EDITOR)

if(BUILD_PEACH_ENGINE)
    ####################################### Find peach engine entry point
    file(
        GLOB_RECURSE 
        PEACH_ENGINE_SOURCES 
        CONFIGURE_DEPENDS 
        "src/peach_engine/main.cpp" #thin wrapper for Peach Engine executable
    )

    ####################################### Create executable target

    if(PEACH_MACOS) #create a bundle instead of a regular exe for macOS
        set(MACOSX_BUNDLE TRUE)

        add_executable(${PROJECT_NAME} MACOSX_BUNDLE
            ${PEACH_ENGINE_SOURCES}
        )
    else()
        add_executable(
            ${PROJECT_NAME} 
            ${PEACH_ENGINE_SOURCES}
        )
    endif(PEACH_MACOS)

    ####################################### Compiler Options and Definitions

    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 

    endif(MSVC)

    ####################################### Link Peach Engine to Peach Core

    target_link_libraries(${PROJECT_NAME} PRIVATE 
        peach_core
    )

    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${PROJECT_SOURCE_DIR}/src/peach_core
    )

endif(BUILD_PEACH_ENGINE)

if(BUILD_PEACH_TESTS)
    file(
        GLOB_RECURSE 
        PEACH_TEST_SOURCES 
        CONFIGURE_DEPENDS 
        "test_suite/*.cpp" 
        "test_suite/*.h"
        "test_suite/*.hpp"
    )

        ####################################### Create executable target

    add_executable( 
        Peach_Tests
        ${PEACH_TEST_SOURCES}
    )

    ####################################### Compiler Options and Definitions

    if(MSVC)
        target_compile_options(Peach_Tests PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 

        target_link_options(Peach_Tests PRIVATE /NODEFAULTLIB:LIBCMTD)
    endif(MSVC)

    ####################################### Link to Peach Core

    target_link_libraries(Peach_Tests PRIVATE 
        peach_core
    )

    target_include_directories(Peach_Tests PRIVATE 
        ${PROJECT_SOURCE_DIR}/src/peach_core
    )

endif(BUILD_PEACH_TESTS)

####################################### Set Startup Project (Visual Studio & Xcode)

# Set Peach_Editor as the default scheme in Xcode

if(CMAKE_GENERATOR MATCHES "Xcode" AND BUILD_PEACH_EDITOR)
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
    set(CMAKE_XCODE_SCHEME_EXECUTABLE Peach_Editor)
    message(STATUS "Setting Peach_Editor as the default startup project for Xcode")
endif()

# Set Peach_Editor as the startup project for Visual Studio

if(CMAKE_GENERATOR MATCHES "Visual Studio" AND BUILD_PEACH_EDITOR)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Peach_Editor)
    message(STATUS "Setting Peach_Editor as the default startup project for VS2022")
    
    set(NETHOST_DLL_PATH "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/debug>")

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_ENVIRONMENT 
        "$<$<CONFIG:Debug>:PATH=${NETHOST_DLL_PATH};%PATH%>"
    )

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "")
endif()
