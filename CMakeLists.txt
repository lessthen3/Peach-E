##########################################################################
#                        Peach-E v0.0.1
#         Created by Ranyodh Singh Mandur - 🍑 2024-2026
#
#              Licensed under the MIT License (MIT).
#         For more details, see the LICENSE file or visit:
#               https://opensource.org/licenses/MIT
#
#           Peach-E is a free open source game engine
##########################################################################

####################################### Enforce CMake Version

cmake_minimum_required(VERSION 4.2.0) #need this for vs2026

####################################### Config Options

option(BUILD_PEACH_CORE_SHARED "Builds peach_core as a shared library" OFF) #build peach_core shared by default uwu

option(BUILD_PEACH_TESTS "build test suite" OFF)
option(BUILD_PEACH_EDITOR "turns on build output for peach editor" ON)
option(BUILD_PEACH_ENGINE "turns on build output for the executable peach engine, doesn't relate to peach_core" OFF)

option(BUILD_ASSET_CONVERTER_TOOL "builds the asset converter exe for importing assets for things like mods" OFF)

####################################### Project definition

project(Peach_Engine VERSION 0.0.1 DESCRIPTION "Peach Engine" LANGUAGES CXX C)

####################################### Enforcing C++20 standard

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################################### Enforce C17 Standard

set(CMAKE_C_STANDARD 17) #needed for box2d owo
set(CMAKE_C_STANDARD_REQUIRED ON)

####################################### Project folders >w<

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "third_party/cmake")

####################################### Build warnings owo

if(MSVC)
    add_compile_options(/W4) # /W4 is the standard "high" warning level for MSVC
else()    
    add_compile_options(-Wall -Wextra -Wpedantic) # Add Compiler Warnings for Clang/GCC (assuming those are the only two ur using on unix like systems uwu)
endif()

# disable third party warnings OwO, idk just trusting em
if(MSVC)
    set(WARNING_OFF_FLAG "/W0")
else()
    set(WARNING_OFF_FLAG "-w") #just assuming clang or gcc if not msvc
endif()

####################################### Check For No Config Set

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "No build type specified, ending build. CMAKE WILL NOW EXIT")
endif()

####################################### Enforce 64-bit Requirement

if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 8 AND NOT PEACH_VITA AND NOT PEACH_WASM) #ah yes browser so good 32 bit got threading in 2020 very smart very good js devs are not terrible owo
    message(FATAL_ERROR "Peach-E only supports 64-bit platforms.")
endif()

####################################### Find peach_core source and headers

# Peach Core
file(
    GLOB_RECURSE 
    PEACH_CORE_SOURCES 
    CONFIGURE_DEPENDS 
    ${PROJECT_SOURCE_DIR}/src/peach_core/*.cpp
    ${PROJECT_SOURCE_DIR}/src/peach_core/*.h
    ${PROJECT_SOURCE_DIR}/src/peach_core/*.hpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/peach_core"
    PREFIX "peach"
    FILES ${PEACH_CORE_SOURCES}
)

####################################### Override build options if we're building for smth weird >///< yes daddy ~~nyah

if(PEACH_ANDROID)

    set(BUILD_PEACH_CORE_SHARED ON  CACHE BOOL "" FORCE)
    set(BUILD_PEACH_EDITOR      OFF CACHE BOOL "" FORCE)
    set(BUILD_PEACH_ENGINE      OFF CACHE BOOL "" FORCE)

    message(STATUS "android detected and forcing peach shared on and turning peach editor and engine off >w< rawr owo")

elseif(PEACH_IOS OR PEACH_TVOS OR PEACH_WASM OR PEACH_VITA)

    set(BUILD_PEACH_CORE_SHARED OFF CACHE BOOL "" FORCE)
    set(BUILD_PEACH_EDITOR      OFF CACHE BOOL "" FORCE)
    set(BUILD_PEACH_ENGINE      ON  CACHE BOOL "" FORCE)

    message(STATUS "ios, tvos, wasm, or vita detected and forcing peach shared and peach editor off, only engine is valid here uwu~~")

endif()

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
    ${PROJECT_SOURCE_DIR}/src/peach_core #because it's prettier and easier for including owo
)

target_include_directories(peach_core SYSTEM PUBLIC #avoid header only warnings ^O^
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/header_only #header only library includes
)

if(PEACH_BSD) #FreeBSD only supports dotnet 9, but pls get that alpha 10 into pkg pwetty pwease owo
    target_include_directories(peach_core SYSTEM PUBLIC
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/include_dotnet9 
    )
else()
    target_include_directories(peach_core SYSTEM PUBLIC 
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/include_dotnet10
    )
endif()

set_target_properties(peach_core PROPERTIES POSITION_INDEPENDENT_CODE ON) #make peach_core PIC friendly UwU

####################################### peach_core platform specific src includes ;w;

####################################### Vulkan driver querying owo

if(PEACH_PLATFORM_IS_WINDOWS OR PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD OR PEACH_HAIKU OR PEACH_ANDROID)

    file(GLOB VOLK_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/plat_dep_source_include/volk/volk.c
    )

    file(GLOB VKBOOTSTRAP_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/plat_dep_source_include/vk-bootstrap/*.cpp
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/plat_dep_source_include/vk-bootstrap/*.inl
    )

    source_group("third_party/volk" FILES ${VOLK_SOURCES})
    source_group("third_party/vkbootstrap" FILES ${VKBOOTSTRAP_SOURCES})

    # disable third party warnings OwO, idk just trusting em
    set_source_files_properties(
        ${VKBOOTSTRAP_SOURCES}
        ${VOLK_SOURCES}
        PROPERTIES COMPILE_FLAGS ${WARNING_OFF_FLAG}
    )

    target_sources(peach_core PRIVATE
        ${VKBOOTSTRAP_SOURCES}
        ${VOLK_SOURCES}
    )

    target_include_directories(peach_core SYSTEM PUBLIC #try to avoid header warnings >o<
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/plat_dep_source_include #include since it passed the plat test
    )
    
endif(PEACH_PLATFORM_IS_WINDOWS OR PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD OR PEACH_HAIKU OR PEACH_ANDROID) #vulkan source includes

####################################### GLEW include

if(PEACH_PLATFORM_IS_WINDOWS OR PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD OR PEACH_HAIKU) #only platforms where glew makes sense ^O^

    file(GLOB GLEW_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/glew.c
    )

    source_group("third_party/glew" FILES ${GLEW_SOURCES})

    # disable third party warnings OwO, idk just trusting em
    set_source_files_properties(
        ${GLEW_SOURCES}
        PROPERTIES COMPILE_FLAGS ${WARNING_OFF_FLAG}
    )

    target_sources(peach_core PRIVATE
        ${GLEW_SOURCES}
    )

    target_include_directories(peach_core SYSTEM PUBLIC #try to avoid header warnings >o<
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/glew/include #need this because GL is kinda weird w glew w/e >w<
    )

endif(PEACH_PLATFORM_IS_WINDOWS OR PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD OR PEACH_HAIKU)

####################################### peach_core src includes

file(GLOB LUA_SOURCES 
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/lua/*.c
)

file(GLOB LZ4_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/lz4/*.c
)

file(GLOB CLIPPER2_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/clipper2/*.cpp
)

file(GLOB BOX2D_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/box2d/*.c
)

file(GLOB_RECURSE JOLT_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/Jolt/*.cpp #need caps since jolt does the target include thing in its cmake owo
)

file(GLOB FMT_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/fmt/format.cc
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/fmt/os.cc
)

file(GLOB MONOCYPHER_SOURCES
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/monocypher/*.c
)

if(PEACH_PLATFORM_IS_APPLE)
    file(GLOB PHYSFS_SOURCES 
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/physfs/*.c
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/physfs/*.m
    )
else()
    file(GLOB PHYSFS_SOURCES 
        ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include/physfs/*.c
    )
endif(PEACH_PLATFORM_IS_APPLE)


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

target_include_directories(peach_core SYSTEM PUBLIC #try to avoid header warnings owo
    ${PROJECT_SOURCE_DIR}/third_party/peach_core/source_include #supported across the board owo ily guys ~~nyah
)

if(PEACH_PLATFORM_IS_APPLE)
    target_sources(peach_core PRIVATE 
        ${PROJECT_SOURCE_DIR}/src/peach_core/rendering/Metal/MetalBridge.mm  #just to get the objc file compiled for the extern "C" call owo
    ) 
endif()

source_group("third_party/physfs" FILES ${PHYSFS_SOURCES})
source_group("third_party/lua" FILES ${LUA_SOURCES})
source_group("third_party/lz4" FILES ${LZ4_SOURCES})
source_group("third_party/clipper2" FILES ${CLIPPER2_SOURCES})
source_group("third_party/box2d" FILES ${BOX2D_SOURCES})
source_group("third_party/Jolt" FILES ${JOLT_SOURCES})
source_group("third_party/fmt" FILES ${FMT_SOURCES})
source_group("third_party/monocypher" FILES ${MONOCYPHER_SOURCES})

# disable third party warnings OwO, idk just trusting em
set_source_files_properties(
    ${PHYSFS_SOURCES}
    ${LUA_SOURCES}
    ${LZ4_SOURCES}
    ${CLIPPER2_SOURCES}
    ${BOX2D_SOURCES}
    ${JOLT_SOURCES}
    ${FMT_SOURCES}
    ${MONOCYPHER_SOURCES}
    PROPERTIES COMPILE_FLAGS ${WARNING_OFF_FLAG}
)

####################################### SDL time for the big guns >w< (pls be gentle ;w;)

# ==============================================================================
# 🍑 VENDOR SDL3 BUILD TREE INTEGRATION
# ==============================================================================

# Force global configuration cache overrides before loading the directory
set(SDL_PRESEED         OFF CACHE BOOL "" FORCE)
set(SDL_SHARED          OFF CACHE BOOL "" FORCE)
set(SDL_STATIC          ON  CACHE BOOL "" FORCE)

# Strike out all testing, documentation, and sample bloat
set(SDL_TEST_LIBRARY    OFF CACHE BOOL "" FORCE)
set(SDL_TESTS           OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES        OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL_DOCS    OFF CACHE BOOL "" FORCE)

# Subsystems kitten actually uses
set(SDL_VIDEO           ON  CACHE BOOL "" FORCE)
set(SDL_JOYSTICK        ON  CACHE BOOL "" FORCE)
set(SDL_HAPTIC          ON  CACHE BOOL "" FORCE)
set(SDL_HIDAPI          ON  CACHE BOOL "" FORCE)
set(SDL_POWER           ON  CACHE BOOL "" FORCE)
set(SDL_SENSOR          ON  CACHE BOOL "" FORCE)

# Subsystems we drop (MiniAudio handles audio, we write our own renderers)
set(SDL_AUDIO           OFF CACHE BOOL "" FORCE)
set(SDL_RENDER          OFF CACHE BOOL "" FORCE)
set(SDL_GPU             OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA          OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG          OFF CACHE BOOL "" FORCE)
set(SDL_DISKAUDIO       OFF CACHE BOOL "" FORCE)
set(SDL_DUMMYAUDIO      OFF CACHE BOOL "" FORCE)
set(SDL_DUMMYVIDEO      OFF CACHE BOOL "" FORCE)
set(SDL_DUMMYCAMERA     OFF CACHE BOOL "" FORCE)
set(SDL_OFFSCREEN       OFF CACHE BOOL "" FORCE)
set(SDL_WASAPI          OFF CACHE BOOL "" FORCE)

####################################### Static Linkage

if(PEACH_PLATFORM_IS_WINDOWS)

    set(SDL_VULKAN      ON  CACHE BOOL "" FORCE)
    set(SDL_DIRECTX     ON  CACHE BOOL "" FORCE)
    set(SDL_OPENGL      ON  CACHE BOOL "" FORCE)

    set(SDL_METAL       OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGLES    OFF CACHE BOOL "" FORCE)

    set(SDL_LIBC        OFF CACHE BOOL "" FORCE)

elseif(PEACH_PLATFORM_IS_MACOS OR PEACH_IOS OR PEACH_TVOS) #peach.toolchain.cmake picks everything properly owo

    set(SDL_METAL       ON  CACHE BOOL "" FORCE)

    set(SDL_OPENGL      OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGLES    OFF CACHE BOOL "" FORCE)
    set(SDL_VULKAN      OFF CACHE BOOL "" FORCE)
    set(SDL_DIRECTX     OFF CACHE BOOL "" FORCE)

elseif(PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT

    set(SDL_VULKAN      ON  CACHE BOOL "" FORCE)
    set(SDL_OPENGL      ON  CACHE BOOL "" FORCE)

    set(SDL_DIRECTX     OFF CACHE BOOL "" FORCE)
    set(SDL_METAL       OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGLES    OFF CACHE BOOL "" FORCE)

    set(SDL_LIBC        ON  CACHE BOOL "" FORCE)

endif()

# Inject the subdirectory >O<
if(NOT TARGET SDL3-static)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/peach_core/SDL EXCLUDE_FROM_ALL)
endif()

# ==============================================================================
# 🧹 SDL3 TARGET VISUAL DECLUTTER OwO
# ==============================================================================
# SDL3's build system creates a few helper targets alongside the main engine library; this loop sweeps them into a clean IDE project subfolder to preserve sanity
foreach(f_SdlTarget 
    SDL3-shared 
    SDL3-static 
    SDL3_test 
    uninstall
)
    if(TARGET ${f_SdlTarget})
        set_target_properties(${f_SdlTarget} PROPERTIES FOLDER "third_party/SDL3")

        target_compile_options(${f_SdlTarget} PRIVATE ${WARNING_OFF_FLAG})

        if(MSVC)
            target_compile_definitions(${f_SdlTarget} PRIVATE _CRT_SECURE_NO_WARNINGS)
        endif()
    endif()
endforeach()

# ==============================================================================
# 🚀 ENFORCE MSVC STATIC RUNTIME ON WINDOWS FOR SDL3
# ==============================================================================
if(MSVC)
    if(TARGET SDL3-static)
        set_property(TARGET SDL3-static PROPERTY MSVC_RUNTIME_LIBRARY
            "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        )
    endif()
endif()

# ==============================================================================
# link!
# ==============================================================================
target_link_libraries(peach_core PUBLIC
    SDL3::SDL3
)

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

####################################### Dependency Linking

if(PEACH_PLATFORM_IS_LINUX)

    find_package(OpenGL REQUIRED COMPONENTS OpenGL GLX EGL)
    
    target_link_libraries(peach_core PUBLIC
        OpenGL::GL
        OpenGL::GLX
        OpenGL::EGL
    )

elseif(PEACH_PLATFORM_IS_WINDOWS)

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        #opengl
        opengl32

        setupapi 
        version 
        imm32 
        winmm 
        kernel32
    )

elseif(PEACH_PLATFORM_IS_FREEBSD)

    find_package(OpenGL REQUIRED)

    target_link_libraries(peach_core PUBLIC
        GL
        usbhid # needed for SDL3
        pthread # FreeBSD uses clang by default, pthread is separate
    )

elseif(PEACH_HAIKU)    

    find_package(OpenGL REQUIRED)

    # Haiku has its own GL implementation
    target_link_libraries(peach_core PUBLIC
        GL
        be      # Haiku's core API, SDL3 needs this
        network # Haiku network stack
    )

elseif(PEACH_ANDROID)
    # System libs provided by the NDK/Android OS itself, no static import needed
    target_link_libraries(peach_core PUBLIC
        android      # ANativeActivity, AAssetManager, etc.
        log          # __android_log_print 
        GLESv3       # OpenGL ES 3.0
        EGL          # EGL surface/context management
        OpenSLES     # Audio (miniaudio fallback since AAudio requires API 26+)
        # Vulkan is loaded dynamically via volk, no -lVulkan needed
    )

    ############# 16KB page-size alignment for Android 15+ #############
    # Play Store mandate as of Nov 2025, 16KB alignment works with the older 4KB page devices, tl;dr this is universally correct on Android, so we set unconditionally
    target_link_options(peach_core PRIVATE
        "-Wl,-z,max-page-size=16384"
        "-Wl,-z,common-page-size=16384"
    )
elseif(PEACH_IOS)

elseif(PEACH_VITA)
    # PS Vita uses .stub files to map to the OS modules. 
    target_link_libraries(peach_core PUBLIC
        # --- Graphics & Display ---
        SceGxm_stub          # The native low-level GPU API
        SceDisplay_stub      # Screen buffers and VSync
        SceShUtility_stub    # Shell utilities (for splash screens/scaling)
        
        # --- Input ---
        SceCtrl_stub         # Buttons and Joysticks
        SceTouch_stub        # Front and Rear touchpads
        SceHid_stub          # Support for external controllers (DS4/PSTV)

        # --- Sensors ---
        SceMotion_stub       # Gyro and Accelerometer
        
        # --- Audio ---
        SceAudio_stub        # Primary audio output
        SceAudiodec_stub     # Hardware decoding

        # --- System Services ---
        ScePower_stub        # Battery info and CPU/GPU clock frequency control (don't forget to boost to 444MHz!)(okay thanks ig) scePowerSetArmClockFrequency(444) scePowerSetGpuClockFrequency(222)
        SceRtc_stub          # Real-time clock
        SceAppUtil_stub      # System app events (OS buttons, battery)
        SceCommonDialog_stub # System keyboard and message popups
        SceSysmodule_stub    # Required to load GXM/Net/etc modules at runtime

        # --- Network ---
        SceNet_stub          # Sockets
        SceNetCtl_stub       # Network state/Wifi
        
        # --- Core Runtime ---
        SceLibKernel_stub    # equivalent of kernel32.dll apparently so yeah, i'd like to allocate memory, might be needed at some point ig owo
        pthread              # VitaSDK's pthread implementation
        m                    # still a dumb fucking name for the math lib but ig its called libm y not libmath or literally anything else LMFAO the C programmer urge to name smth in the least descriptive way possible >w<
    )

    # Vita linker requires -Wl,-q to keep relocations for the .velf tool
    target_link_options(peach_core PRIVATE "-Wl,-q")

    # Create the Vita ELF
    add_custom_command(TARGET peach_core POST_BUILD
        COMMAND vita-elf-create $<TARGET_FILE:peach_core> peach_core.velf
    )

    # "sign" it for homebrew
    add_custom_command(TARGET peach_core POST_BUILD
        COMMAND vita-make-fself peach_core.velf eboot.bin
    )

    # Create the System File Object
    add_custom_command(TARGET peach_core POST_BUILD
        COMMAND vita-mksfoex -s TITLE_ID="PEACH0001" "Peach Engine" param.sfo
    )

    # Pack the VPK (I'll need an assets folder with a sce_sys/icon0.png apparently)
    add_custom_command(TARGET peach_core POST_BUILD
        COMMAND vita-pack-vpk -s param.sfo -b eboot.bin peach_engine.vpk
    )

endif()

####################################### Master snuggle list for platform detection UwU (my chatgpt is bricked af)
# daddy guarantees exactly ONE of these is always defined per build so no ambiguity 🍑
# daddy guaranteed so ig it works OwO

peach_apply_platform_definitions(peach_core PUBLIC)

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
    target_compile_definitions(peach_core PUBLIC  #dam switch u guys make some amazing hardware full respects o7
        PEACH_RENDERER_OPENGL
        PEACH_RENDERER_OPENGL_ES
        PEACH_RENDERER_VULKAN
    )

elseif(PEACH_PLATFORM_IS_MACOS OR PEACH_IOS OR PEACH_TVOS)
    target_compile_definitions(peach_core PUBLIC
        PEACH_RENDERER_METAL
    )

elseif(PEACH_PLATFORM_IS_WINDOWS OR PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD OR PEACH_HAIKU)
    target_compile_definitions(peach_core PUBLIC
        PEACH_RENDERER_VULKAN
        PEACH_RENDERER_OPENGL
    )

elseif(PEACH_ANDROID)
    target_compile_definitions(peach_core PUBLIC
        PEACH_RENDERER_VULKAN
        PEACH_RENDERER_OPENGL_ES
    )

endif()

####################################### Compiler Options and Other Definitions

target_compile_definitions(peach_core PUBLIC 
    $<$<CONFIG:Debug>:PEACH_DEBUG>
    GLEW_NO_GLU #is not used on mac but is whatever just gets ignored anyways owo
)

if(NOT PEACH_PLATFORM_IS_WINDOWS)
    target_compile_definitions(peach_core PUBLIC LUA_USE_POSIX) #so that lua uses mkstemp instead for atomic file whatevers ig
endif()

# IMPORTANT: suppress SDL's main() rename on platforms where we own main()., Android MUST NOT define this, SDL's JNI bridge NEEDS THIS
if(NOT PEACH_ANDROID AND NOT PEACH_IOS AND NOT PEACH_TVOS)
    target_compile_definitions(peach_core PUBLIC SDL_MAIN_HANDLED)
endif()

if(PEACH_PLATFORM_IS_WINDOWS OR PEACH_PLATFORM_IS_LINUX OR PEACH_PLATFORM_IS_FREEBSD OR PEACH_HAIKU OR PEACH_ANDROID)

    target_compile_definitions(peach_core PRIVATE #needs to be private otherwise will leak into the editor which needs vulkan.h for cross 
        VK_NO_PROTOTYPES
    ) # IMPORTANT: tells volk to not use the default vulkan loader

endif()

if(PEACH_VITA OR PEACH_WASM)
    target_compile_definitions(peach_core PUBLIC #vita can't do SIMD w Box2D too old ;w;
        BOX2D_DISABLE_SIMD
    )
endif(PEACH_VITA OR PEACH_WASM)

if(PEACH_PLATFORM_IS_WINDOWS)

    target_compile_definitions(peach_core PUBLIC
        GLEW_STATIC #needed for statically linking glew
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )

    if(MSVC)
        target_compile_options(peach_core PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
        ) 
        
        target_compile_options(peach_core PUBLIC 
            /utf-8
            /Zc:preprocessor
        )
    endif(MSVC)

endif(PEACH_PLATFORM_IS_WINDOWS)

if(BUILD_PEACH_CORE_SHARED)
    target_compile_definitions(peach_core PRIVATE
        PEACH_BUILD_DYNAMIC
    )     
else()
    target_compile_definitions(peach_core PRIVATE
        PEACH_API_STATIC #used for compiling peach_api as a static lib uwu
    )     
endif(BUILD_PEACH_CORE_SHARED)

############################################################################## Peach Engine

if(BUILD_PEACH_ENGINE)

    ####################################### Find peach engine entry point

    if(PEACH_WASM)
        set(PEACH_ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/src/peach_core/platform_entry_points/WasmMain.cpp)
    else()
        file(
            GLOB 
            PEACH_ENGINE_SOURCES 
            CONFIGURE_DEPENDS 
            ${PROJECT_SOURCE_DIR}/src/peach_engine/main.cpp #thin wrapper for Peach Engine executable
        )

        source_group(
            TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/peach_engine"
            PREFIX "peach_engine"
            FILES ${PEACH_ENGINE_SOURCES}
        )
    endif()

    ####################################### Create executable target

    if(PEACH_PLATFORM_IS_MACOS) #create a bundle instead of a regular exe for macOS
        set(MACOSX_BUNDLE TRUE)

        add_executable(${PROJECT_NAME} MACOSX_BUNDLE
            ${PEACH_ENGINE_SOURCES}
        )
    else()
        add_executable(${PROJECT_NAME} 
            ${PEACH_ENGINE_SOURCES}
        )
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON) #make Peach_Engine PIC friendly UwU

    ####################################### Wasm stuff owo

    if(PEACH_WASM)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            SUFFIX ".html"  # emscripten also produces .js and .wasm alongside it
        )

        target_link_options(${PROJECT_NAME} PRIVATE
            "-sMAX_WEBGL_VERSION=2"
            "-sUSE_SDL=3"
            "-sFULL_ES3=1"                  # full OpenGL ES 3.0 API surface
            "-sALLOW_MEMORY_GROWTH=1"       # heap can expand as needed, important for engine
            "-sINITIAL_MEMORY=67108864"     # 64MB initial heap, adjust later
            "-sPTHREAD_POOL_SIZE=4"         # pre-spawn 4 workers for std::thread
            "-sEXIT_RUNTIME=0"              # don't destroy runtime when main() returns — rAF loop keeps running
            "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']" # expose for potential JS interop
        )

        # WasmMain.cpp uses emscripten headers so the library also needs the include path
        target_include_directories(${PROJECT_NAME} PRIVATE
            "$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include"
        )
    endif()

    ####################################### Compiler Options and Definitions

    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
            /Zc:preprocessor #for va opt owo
        ) 
    endif(MSVC)

    ####################################### Link Peach Engine to Peach Core

    target_link_libraries(${PROJECT_NAME} PRIVATE 
        peach_core
    )

endif(BUILD_PEACH_ENGINE)

############################################################################## Peach Editor

if(BUILD_PEACH_EDITOR)

    ####################################### Find Peach_Editor source and headers

    file(
        GLOB_RECURSE 
        PEACH_EDITOR_SOURCES 
        CONFIGURE_DEPENDS 
        ${PROJECT_SOURCE_DIR}/src/peach_editor/*.cpp
        ${PROJECT_SOURCE_DIR}/src/peach_editor/*.h
        ${PROJECT_SOURCE_DIR}/src/peach_editor/*.hpp
    )

    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/peach_editor"
        PREFIX "peach_editor"
        FILES ${PEACH_EDITOR_SOURCES}
    )

    ####################################### Create executable target

    if(PEACH_PLATFORM_IS_MACOS) #create a bundle instead of a regular exe for macOS
        set(MACOSX_BUNDLE TRUE)

        add_executable(Peach_Editor MACOSX_BUNDLE
            ${PEACH_EDITOR_SOURCES}
        )
    else()
        # Peach Editor
        add_executable(Peach_Editor
            ${PEACH_EDITOR_SOURCES}
        )
    endif()

    target_include_directories(Peach_Editor PRIVATE
        ${PROJECT_SOURCE_DIR}/src/peach_editor/ #again because its prettier owo
    )

    set_target_properties(Peach_Editor PROPERTIES POSITION_INDEPENDENT_CODE ON) #make Peach_Editor PIC friendly UwU

    ####################################### pseudo source cmake include owo

    file(GLOB TINYXML2_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_editor/source_include/tinyxml2/tinyxml2.cpp
    )

    file(GLOB LODEPNG_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_editor/source_include/lodepng/lodepng.cpp
    )

    file(GLOB SPIRV_CROSS_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_editor/weird_source_include/spirv_cross/*.cpp
    )

    file(GLOB BC7ENC_RDO_SOURCES
        ${PROJECT_SOURCE_DIR}/third_party/peach_editor/source_include/bc7enc_rdo/*.cpp
    )

    target_sources(Peach_Editor PRIVATE
        ${TINYXML2_SOURCES}
        ${LODEPNG_SOURCES}
        ${SPIRV_CROSS_SOURCES}
        ${BC7ENC_RDO_SOURCES}
    )

    target_include_directories(Peach_Editor SYSTEM PRIVATE
        third_party/peach_editor/source_include
        third_party/peach_editor/weird_source_include/spirv_cross        # root hpp files
        third_party/peach_editor/weird_source_include/spirv_cross/include # vendored spirv-headers
    )

    source_group("third_party/tinyxml2" FILES ${TINYXML2_SOURCES})
    source_group("third_party/lodepng" FILES ${LODEPNG_SOURCES})
    source_group("third_party/spirv_cross" FILES ${SPIRV_CROSS_SOURCES})
    source_group("third_party/bc7enc_rdo" FILES ${BC7ENC_RDO_SOURCES})

    # disable third party warnings OwO, idk just trusting em
    set_source_files_properties(
        ${TINYXML2_SOURCES}
        ${LODEPNG_SOURCES}
        ${SPIRV_CROSS_SOURCES}
        ${BC7ENC_RDO_SOURCES}
        PROPERTIES COMPILE_FLAGS ${WARNING_OFF_FLAG} #WARNING off flag is always set since peach_core is always built by default owo
    )

    ####################################### tell astcenc which ISA to build for based on our platform

    set(ASTCENC_CLI             OFF CACHE BOOL "" FORCE) # dont build the command line tool
    set(ASTCENC_SHAREDLIB       OFF CACHE BOOL "" FORCE)
    set(ASTCENC_DYNAMIC_RUNTIME OFF CACHE BOOL "" FORCE)

    if(PEACH_ARCH_ARM64)

        set(ASTCENC_ISA_NEON  ON  CACHE BOOL "" FORCE)
        set(ASTCENC_ISA_AVX2  OFF CACHE BOOL "" FORCE)
        set(ASTCENC_ISA_SSE41 OFF CACHE BOOL "" FORCE)
        set(ASTCENC_ISA_SSE2  OFF CACHE BOOL "" FORCE)

        set(PEACH_USING_NEON_ASTCENC TRUE)

    elseif(PEACH_ARCH_X64)

        set(ASTCENC_ISA_NEON  OFF CACHE BOOL "" FORCE)
        set(ASTCENC_ISA_AVX2  OFF CACHE BOOL "" FORCE)
        set(ASTCENC_ISA_SSE41 ON  CACHE BOOL "" FORCE)
        set(ASTCENC_ISA_SSE2  OFF CACHE BOOL "" FORCE)

        set(PEACH_USING_NEON_ASTCENC FALSE)

    endif()

    if(NOT TARGET astcenc-neon-static AND NOT TARGET astcenc-sse4.1-static)
        add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/peach_editor/source_include/astcenc)
    endif()

    ####################################### astenc project folder visual declutter OwO UwU

    foreach(f_CTestTarget Experimental Nightly Continuous NightlyMemoryCheck)
        if(TARGET ${f_CTestTarget})
            set_property(TARGET ${f_CTestTarget} PROPERTY FOLDER "third_party/astenc/ctests")
        endif()
    endforeach()

    # clean up all possible targets >w<
    set(f_AstcPossibleTargets

        astcenc-neon-static   astcenc-sse4.1-static 
        astcenc-avx2-static   astcenc-sse2-static
        astcenc-native-static astcenc-none-static
    )

    foreach(f_AstcTarget ${f_AstcPossibleTargets})
        if(TARGET ${f_AstcTarget})

            set_target_properties(${f_AstcTarget} PROPERTIES FOLDER "third_party/astenc") #shove outta site uwu but thanks for the code ^W^

            if(MSVC)   #XXX: set MT or MTd for the msvc runtime for astenc owo
                set_property(TARGET ${f_AstcTarget} PROPERTY MSVC_RUNTIME_LIBRARY
                    MultiThreaded$<$<CONFIG:Debug>:Debug>
                )
            endif()

        endif()
    endforeach()

    ####################################### 

    target_link_libraries(Peach_Editor PRIVATE
        $<IF:$<BOOL:${PEACH_USING_NEON_ASTCENC}>,astcenc-neon-static,astcenc-sse4.1-static>
    )   

    # ==============================================================================
    # 🔮 VENDOR SHADERC STATIC BUILD TREE INTEGRATION (these stubs are cute i like them emojis are cool uwu owo)
    # ==============================================================================

    # Core Shaderc Flag Overrides
    set(SHADERC_SKIP_TESTS           ON   CACHE BOOL "" FORCE)
    set(SHADERC_SKIP_EXAMPLES        ON   CACHE BOOL "" FORCE)
    set(SHADERC_SKIP_COPYRIGHT_CHECK ON   CACHE BOOL "" FORCE)
    set(SHADERC_ENABLE_SHARED_CRT    OFF  CACHE BOOL "" FORCE) # Enforces static CRT
    set(SHADERC_ENABLE_HLSL          OFF  CACHE BOOL "" FORCE) # Peach only uses open specs
    set(SHADERC_SKIP_EXECUTABLES     OFF  CACHE BOOL "" FORCE) # yeah just put the fries in the bag man, no exes 
    set(BUILD_SHARED_LIBS            OFF  CACHE BOOL "" FORCE)

    # Spirv-Tools and glslang
    set(ENABLE_GLSLANG_BINARIES      OFF  CACHE BOOL "" FORCE) # no standalone glslang exe
    set(SPIRV_SKIP_EXECUTABLES       ON   CACHE BOOL "" FORCE) # Kills spirv-dis / spirv-as CLIs
    set(SPIRV_TOOLS_BUILD_STATIC     ON   CACHE BOOL "" FORCE)
    set(GLSLANG_TESTS                OFF  CACHE BOOL "" FORCE) # weird they use two opposing option logics but w/e google is beeg 
    set(SPIRV_SKIP_TESTS             ON   CACHE BOOL "" FORCE)
    set(SKIP_SPIRV_TOOLS_INSTALL     ON   CACHE BOOL "" FORCE) # uh okay ig i think this'll be fine

    # force static crt, idk i don't trust the enabled shared crt flag idk if it propagates down properly but w/e doesn't hurt
    if(PEACH_PLATFORM_IS_WINDOWS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
    endif()

    if(NOT TARGET shaderc_combined) # immutable guard so that we don't recompile uwu
        add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/peach_editor/shaderc EXCLUDE_FROM_ALL SYSTEM) # Inject the subdirectory (>O< thanks googy woogy >///<)
    endif()

    # ==============================================================================
    # 🧹 SHADERC TARGET EXTINCTION & DE-CLUTTER OwO
    # ==============================================================================
    # Shaderc creates ~30 sub-targets; We loop through all known sub-dependency components to SLAM them into a hidden IDE workspace folder and enforce uniform /MT configurations
    set(f_ShadercInternalTargets

        shaderc shaderc_util shaderc_combined
        glslang OSDependent 
        MachineIndependent GenericCodeGen SPIRV
        SPIRV-Tools SPIRV-Tools-shared SPIRV-Tools-static 
        SPIRV-Tools-opt SPIRV-Tools-link SPIRV-Tools-reduce

        spirv-tools-tables core_tables extinst_tables
        spirv-tools-header-DebugInfo spirv-tools-header-OpenCLDebugInfo100
        spirv-tools-header-NonSemanticShaderDebugInfo100 
        
        spirv-tools-header-NonSemanticClspvReflection enum_string_mapping #unsure ab these ones but w/e
    )

    foreach(f_SubTarget ${f_ShadercInternalTargets})

        if(TARGET ${f_SubTarget}) # sub target :^)
            
            get_target_property(f_RealTargetName  ${f_SubTarget} ALIASED_TARGET) # Check if this target name is an ALIAS, idfk im way too lazy to read the 50 cmakelists and .cmake whatever the fucks
            
            if(NOT f_RealTargetName )

                set_target_properties(${f_SubTarget} PROPERTIES FOLDER "third_party/shaderc_internals") # Sweeps the project windows completely out of sight into a subfolder
                get_target_property(f_TargetType ${f_SubTarget} TYPE)

                # Enforce static runtime library link constraints unconditionally on MSVC windows hosts, no universal crt selection flag sadly ;A;
                if(MSVC AND NOT f_TargetType STREQUAL "UTILITY")
                    set_property(TARGET ${f_SubTarget} PROPERTY MSVC_RUNTIME_LIBRARY #didn't know i could just do MSVC_RUNTIME_LIBRARY cool
                        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
                    )
                endif()
            endif()
        endif()

    endforeach()

    # =================================================================================================
    # Silence Warnings for shaderc because i trust google they'd never do anything bad right (right?)
    # =================================================================================================

    foreach(f_Target shaderc_util shaderc_combined shaderc)
        if(TARGET ${f_Target})
            target_compile_options(${f_Target} PRIVATE ${WARNING_OFF_FLAG})
            if(MSVC)
                target_compile_definitions(${f_Target} PRIVATE _CRT_SECURE_NO_WARNINGS)
            endif()
        endif()
    endforeach()

    # =================================================================================================
    # link and find includes for peach editor uwu
    # =================================================================================================

    target_link_libraries(Peach_Editor PRIVATE
        shaderc_combined
    )   
    # have to manually declare these since it isnt transitively pulled in 
    target_include_directories(Peach_Editor PRIVATE 
        ${PROJECT_SOURCE_DIR}/third_party/peach_editor/shaderc/libshaderc/include
        ${PROJECT_SOURCE_DIR}/third_party/peach_editor/shaderc/third_party/spirv-tools/include
    )

    # ==============================================================================
    # 🔠 msdfgen, freetype and tinyxml2 so smol >O<
    # ==============================================================================

    # tinyxml2 overrides
    set(tinyxml2_BUILD_TESTING    OFF CACHE BOOL "" FORCE)
    set(tinyxml2_SHARED_LIBS      OFF CACHE BOOL "" FORCE) # just like being explicit idk

    # freetype overrides
    set(FT_DISABLE_ZLIB           ON CACHE BOOL "" FORCE) # Keep zlib for compressed fonts
    set(FT_DISABLE_BZIP2          ON  CACHE BOOL "" FORCE) # Kill ancient compression bloat
    set(FT_DISABLE_PNG            ON  CACHE BOOL "" FORCE) # Kill color emoji font bloat
    set(FT_DISABLE_HARFBUZZ       ON  CACHE BOOL "" FORCE) # Kill complex layout text bloat
    set(FT_DISABLE_BROTLI         ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HVF            ON CACHE BOOL "" FORCE)
    set(FT_ENABLE_ERROR_STRINGS   OFF  CACHE BOOL "" FORCE)

    set(SKIP_INSTALL_HEADERS           ON CACHE BOOL "" FORCE) # tell freetype to shut the fuck up about installs uwu
    set(SKIP_INSTALL_LIBRARIES         ON CACHE BOOL "" FORCE)
    set(SKIP_INSTALL_ALL               ON CACHE BOOL "" FORCE)

    # msdfgen overrides
    set(MSDFGEN_CORE_ONLY            OFF  CACHE BOOL "" FORCE) # build ext too, we need font loading
    set(MSDFGEN_DISABLE_PNG          ON   CACHE BOOL "" FORCE) # drop libpng dep, we use LodePNG ourselves
    set(MSDFGEN_DISABLE_SVG          OFF  CACHE BOOL "" FORCE) # Keep SVG support active
    set(MSDFGEN_BUILD_STANDALONE     OFF  CACHE BOOL "" FORCE) # no standalone exe
    set(MSDFGEN_INSTALL              OFF  CACHE BOOL "" FORCE) # its off by default but y not
    set(MSDFGEN_USE_SKIA             OFF  CACHE BOOL "" FORCE) # no self-intersection resolution via skia
    set(MSDFGEN_USE_OPENMP           OFF  CACHE BOOL "" FORCE) # single threaded is fine for offline pipeline
    set(MSDFGEN_USE_VCPKG            OFF  CACHE BOOL "" FORCE) # Ew, gross

    if(NOT TARGET msdfgen-ext) #probably good enough to guard all, its the neediest dep ;w;
        # Load the dependency tree in order, IMPORTANT: freetype and tinyxml2 must be loaded first so msdfgen can discover their targets since it depends on em owo
        add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/peach_editor/freetype    EXCLUDE_FROM_ALL SYSTEM)

        add_library(Freetype::Freetype ALIAS freetype) #msdfgen is kinda stupid and looks for a precompiled freetype so needa do this owo

        add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/peach_editor/tinyxml2    EXCLUDE_FROM_ALL SYSTEM)
        add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/peach_editor/msdfgen     EXCLUDE_FROM_ALL SYSTEM)
    endif()

    # declutter and set static crt for windows
    set(f_MsdfgenTargets 
        freetype tinyxml2 msdfgen-core msdfgen-ext
    )

    foreach(f_FontTarget ${f_MsdfgenTargets})
        if(TARGET ${f_FontTarget})
            
            set_target_properties(${f_FontTarget} PROPERTIES FOLDER "third_party/msdfgen_stuff") # Sweep loose targets into an IDE folder view shortcut
            
            target_compile_options(${f_FontTarget} PRIVATE ${WARNING_OFF_FLAG}) #turn off warnings for now >w<
            
            if(MSVC)
                target_compile_definitions(${f_FontTarget} PRIVATE _CRT_SECURE_NO_WARNINGS) #turn off warnings nothing im gonna contrib atm until i find some bad errors testing uwu

                set_property(TARGET ${f_FontTarget} PROPERTY MSVC_RUNTIME_LIBRARY # Enforce static runtime library link constraints on MSVC windows hosts
                    "MultiThreaded$<$<CONFIG:Debug>:Debug>"
                )
            endif()
        endif()
    endforeach()

    # linky winkys i need my binky ;w;
    target_link_libraries(Peach_Editor PRIVATE 
        msdfgen::msdfgen-ext
        msdfgen::msdfgen-core
        freetype   
        tinyxml2
    )


    ####################################### Import static libs

    if(PEACH_WINDOWS_X64)

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/release/libnethost.lib" #juicy static linkage UwU
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/debug/nethost.lib" #debug dll
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet10"
        )

    elseif(PEACH_WINDOWS_ARM64)

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64_arm/release/libnethost.lib" #juicy static linkage UwU
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/win64_arm/debug/nethost.lib" #debug dll
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet10"
        )

    elseif(PEACH_PLATFORM_IS_MACOS) #all the macos libs are fat and handle x64 and arm but the exec target will be only for 1 arch UwU!

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/steve64/libnethost_universal.a" #is a fat binary ^O^
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/steve64/libnethost_universal.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet10"
        )

    elseif(PEACH_LINUX_X64) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet10"
        )

    elseif(PEACH_LINUX_ARM64) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64_arm/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/tux64_arm/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet10"
        )

    elseif(PEACH_BSD_X64) #XXX: ;A;

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/freebsd64/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/freebsd64/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet9"
        )
    
    elseif(PEACH_BSD_ARM64) #XXX: ;A;

        ############ dotnet stuff

        STATIC_IMPORT(nethost 
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/freebsd64_arm/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/freebsd64_arm/libnethost.a"
            "${PROJECT_SOURCE_DIR}/third_party/peach_editor/dotnet/include_dotnet9"
        )

    else()
        message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH EDITOR IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX/FREEBSD ON ARM64 AND X64 SORRY NOT SORRY")
    endif()

    ####################################### Compiler Options and Definitions

    if(PEACH_PLATFORM_IS_WINDOWS)
        target_compile_definitions(Peach_Editor PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:NETHOST_USE_AS_STATIC> #need this for nethost to be used statically
        )

        if(MSVC)
            target_compile_options(Peach_Editor PRIVATE
                $<$<CONFIG:Debug>:/MTd>
                $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
                /Zc:preprocessor
            ) 

            target_link_options(Peach_Editor PRIVATE 
                $<$<NOT:$<CONFIG:Debug>>:/LTCG> #because nethost was compiled w /GL w/e ig its gonna happen anyways owo
            )
        endif()

    endif()

    ####################################### Linking deps + peach_core

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
    )

endif(BUILD_PEACH_EDITOR)

############################################################################## Peach Tests owo

if(BUILD_PEACH_TESTS)
    file(
        GLOB_RECURSE 
        PEACH_TEST_SOURCES 
        CONFIGURE_DEPENDS 
        ${PROJECT_SOURCE_DIR}/test_suite/cpp/*.cpp 
        ${PROJECT_SOURCE_DIR}/test_suite/cpp/*.h
        ${PROJECT_SOURCE_DIR}/test_suite/cpp/*.hpp
    )

        ####################################### Create executable target

    add_executable( 
        Peach_Tests
        ${PEACH_TEST_SOURCES}
    )

    set_target_properties(Peach_Tests PROPERTIES POSITION_INDEPENDENT_CODE ON) #make Peach_Tests PIC friendly UwU

    ####################################### Compiler Options and Definitions

    if(MSVC)
        target_compile_options(Peach_Tests PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
            /Zc:preprocessor
        ) 
    endif(MSVC)

    ####################################### Link to Peach Core

    target_link_libraries(Peach_Tests PRIVATE 
        peach_core
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
    
    if(PEACH_WINDOWS_X64)
        set(NETHOST_DLL_PATH "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/third_party/peach_editor/dotnet/win64/debug>")
    elseif(PEACH_WINDOWS_ARM64)
        set(NETHOST_DLL_PATH "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/third_party/peach_editor/dotnet/win64_arm/debug>")
    else()
        message(FATAL "unsupported visual studio ISA found, im assuming you're on windows right?")
    endif()

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_ENVIRONMENT 
        "$<$<CONFIG:Debug>:PATH=${NETHOST_DLL_PATH};%PATH%>"
    )

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "")
endif()
