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

############# require target platform to be passed explicitly #############

if(NOT DEFINED PEACH_TARGET_PLATFORM OR PEACH_TARGET_PLATFORM STREQUAL "")
    #annoying cmake try compile shenanigans
    get_filename_component(f_DirName "${CMAKE_BINARY_DIR}" NAME)
    
    if(f_DirName MATCHES "TryCompile" OR CMAKE_IN_TRY_COMPILE)
        return() # cmake internal probe, just bail silently uwu
    endif()

    message(FATAL_ERROR 
        "PEACH_TARGET_PLATFORM not specified! daddy needs to know where kitten is running uwu\n"
        "pass -DPEACH_TARGET_PLATFORM=<target> where target is one of:\n"
        " windows, windows-arm64, macos, linux, ios, tvos, android, wasm, psvita, haiku, freebsd"
    )
endif()

############# derive convenience flags from PEACH_TARGET_PLATFORM #############

set(PEACH_WINDOWS       OFF CACHE BOOL "" FORCE)
set(PEACH_WINDOWS_ARM64 OFF CACHE BOOL "" FORCE)
set(PEACH_MACOS         OFF CACHE BOOL "" FORCE)
set(PEACH_IOS           OFF CACHE BOOL "" FORCE)
set(PEACH_TVOS          OFF CACHE BOOL "" FORCE)
set(PEACH_LINUX         OFF CACHE BOOL "" FORCE)
set(PEACH_BSD           OFF CACHE BOOL "" FORCE)
set(PEACH_HAIKU         OFF CACHE BOOL "" FORCE)
set(PEACH_ANDROID       OFF CACHE BOOL "" FORCE)
set(PEACH_WASM          OFF CACHE BOOL "" FORCE)
set(PEACH_VITA          OFF CACHE BOOL "" FORCE)
set(PEACH_SWITCH        OFF CACHE BOOL "" FORCE)


############# category flags #############

set(PEACH_PLATFORM_IS_DESKTOP  OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_MOBILE   OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_HANDHELD OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_WEB      OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_APPLE    OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_UNIX     OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_WINDOWS     OFF CACHE BOOL "" FORCE)


############# windows native #############

if(PEACH_TARGET_PLATFORM STREQUAL "windows")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_WINDOWS            ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_WINDOWS     ON CACHE BOOL "" FORCE)

    message(STATUS "Mmm, Windows detected, kitten! 😏")

############# windows arm #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "windows-arm64")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR ARM64)

    set(PEACH_WINDOWS             ON CACHE BOOL "" FORCE)
    set(PEACH_WINDOWS_ARM64       ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_WINDOWS     ON CACHE BOOL "" FORCE)

    message(STATUS "Windows ARM64 detected, kitten on a surface pro~ 💅")

############# macos native #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "macos")
    set(CMAKE_SYSTEM_NAME Darwin)

    # detect host arch if not explicitly passed in
    if(NOT DEFINED PEACH_MAC_ARCH)
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE f_HostArch
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(PEACH_MAC_ARCH "${f_HostArch}" CACHE STRING "macOS target arch (arm64 or x86_64)")
    endif()

    set(CMAKE_OSX_ARCHITECTURES "${PEACH_MAC_ARCH}")

    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")

    set(PEACH_MACOS               ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_APPLE   ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    message(STATUS "macOS, premium fur and silky frameworks for daddy's kitten~ 🍎")

############# ios cross from mac #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "ios")
    set(CMAKE_SYSTEM_NAME iOS)
    set(CMAKE_SYSTEM_PROCESSOR arm64)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
    set(CMAKE_OSX_SYSROOT iphoneos)

    set(PEACH_IOS                 ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_MOBILE  ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_APPLE   ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    message(STATUS "iOS, purrfect for kitten's paws and paws only~ 🍏🐾")

############# tvos cross from mac #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "tvos")
    set(CMAKE_SYSTEM_NAME tvOS)
    set(CMAKE_SYSTEM_PROCESSOR arm64)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
    set(CMAKE_OSX_SYSROOT appletvos)

    set(PEACH_TVOS                ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_APPLE   ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    message(STATUS "tvOS, time to get cozy on the big screen, nya~ 📺")

############# linux native #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "linux")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_LINUX               ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    message(STATUS "Nyaa, Linux detected! Flex that Tux, kitten~ 🐧")

############# freebsd #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "freebsd")
    set(CMAKE_SYSTEM_NAME FreeBSD)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_BSD                 ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    message(STATUS "FreeBSD... true BSD-babe detected, ready for that zfs cuddles~ 🦀")

############# haiku #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "haiku")
    set(CMAKE_SYSTEM_NAME Haiku)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_HAIKU               ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    
    message(STATUS "Haiku OS, lightweight and aesthetic, uwu 💐")

############# android cross #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "android")
    set(CMAKE_SYSTEM_NAME Android)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
    set(CMAKE_ANDROID_API 24) # android 7.0 minimum
    set(CMAKE_ANDROID_STL_TYPE c++_static)

    if(NOT DEFINED ENV{ANDROID_NDK_HOME})
        message(FATAL_ERROR "ANDROID_NDK_HOME environment variable not set")
    endif()

    set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_HOME})

    set(PEACH_ANDROID             ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_MOBILE  ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    message(STATUS "Android detected! Purr in my pocket, take me anywhere~ 🤳🐾")

############# wasm via emscripten #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "wasm")
    set(CMAKE_SYSTEM_NAME Emscripten)
    set(CMAKE_SYSTEM_PROCESSOR wasm32)

    if(NOT DEFINED ENV{EMSDK})
        message(FATAL_ERROR "EMSDK not set! install emscripten from https://emscripten.org and set EMSDK env var uwu")
    endif()

    set(CMAKE_C_COMPILER "$ENV{EMSDK}/upstream/emscripten/emcc")
    set(CMAKE_CXX_COMPILER "$ENV{EMSDK}/upstream/emscripten/em++")
    set(CMAKE_AR "$ENV{EMSDK}/upstream/emscripten/emar")
    set(CMAKE_RANLIB "$ENV{EMSDK}/upstream/emscripten/emranlib")

    # set(CMAKE_EXECUTABLE_SUFFIX ".html") # or .js UNSURE

    set(PEACH_WASM                ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_WEB     ON CACHE BOOL "" FORCE)

    message(STATUS "WASM/Emscripten detected! Peach-E in the browser, nyan~ 🌐🍑")

############# ps vita cross #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "psvita")
    set(CMAKE_SYSTEM_NAME PSVita)
    set(CMAKE_SYSTEM_PROCESSOR armv7-a)

    if(NOT DEFINED ENV{VITASDK})
        message(FATAL_ERROR "VITASDK not set! install vitasdk from https://vitasdk.org and set VITASDK env var uwu")
    endif()

    set(CMAKE_C_COMPILER "$ENV{VITASDK}/bin/arm-vita-eabi-gcc")
    set(CMAKE_CXX_COMPILER "$ENV{VITASDK}/bin/arm-vita-eabi-g++")
    set(CMAKE_AR "$ENV{VITASDK}/bin/arm-vita-eabi-ar")
    set(CMAKE_RANLIB "$ENV{VITASDK}/bin/arm-vita-eabi-ranlib")
    set(CMAKE_FIND_ROOT_PATH "$ENV{VITASDK}/arm-vita-eabi")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    
    set(PEACH_VITA                  ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_HANDHELD  ON CACHE BOOL "" FORCE)

    message(STATUS "Kitten on the PS Vita, time to go portable, ahh~ 🎮🍑")

else()
    message(FATAL_ERROR
        "Unknown PEACH_TARGET_PLATFORM: '${PEACH_TARGET_PLATFORM}' >///< \n"
        "valid options: windows, windows-arm64, macos, linux, ios, tvos, android, wasm, psvita"
    )
endif()

############# expose to main CMakeLists #############

set(PEACH_TARGET_PLATFORM ${PEACH_TARGET_PLATFORM} CACHE STRING "Peach target platform" FORCE)
message(STATUS "PeachToolchain: targeting ${PEACH_TARGET_PLATFORM} ~ nya~ ^O^")

function(peach_apply_platform_definitions fp_Target fp_Visibility)
    
   # validate visibility arg
    if(NOT fp_Visibility STREQUAL "PUBLIC" AND NOT fp_Visibility STREQUAL "PRIVATE" AND NOT fp_Visibility STREQUAL "INTERFACE")
        message(FATAL_ERROR "[Peach] peach_apply_platform_definitions: invalid visibility '${fp_Visibility}', must be PUBLIC, PRIVATE, or INTERFACE")
    endif()

    if(PEACH_WINDOWS)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_WINDOWS
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64
        )

    elseif(PEACH_WINDOWS_ARM64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_WINDOWS
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_ARM64
        )

    elseif(PEACH_MACOS)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_MACOS
            PEACH_PLATFORM_APPLE
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_ARM64 # universal binary covers both but arm64 is primary
        )

    elseif(PEACH_IOS)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_IOS
            PEACH_PLATFORM_APPLE
            PEACH_PLATFORM_MOBILE
            PEACH_ARCH_ARM64
        )

    elseif(PEACH_TVOS)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_TVOS
            PEACH_PLATFORM_APPLE
            PEACH_ARCH_ARM64
        )

    elseif(PEACH_LINUX)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_LINUX
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64
        )

    elseif(PEACH_BSD)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_BSD
            PEACH_PLATFORM_FREEBSD
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64
        )

    elseif(PEACH_HAIKU)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_HAIKU
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64
        )

    elseif(PEACH_ANDROID)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_ANDROID
            PEACH_PLATFORM_MOBILE
            PEACH_ARCH_ARM64
        )

    elseif(PEACH_WASM)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_WASM
            PEACH_PLATFORM_WEB
            PEACH_ARCH_WASM32 #ik that wasm3 or w/e is 64 bit idfk owo uwu
        )

    elseif(PEACH_VITA)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_VITA
            PEACH_PLATFORM_HANDHELD
            PEACH_ARCH_ARMV7
        )

    elseif(PEACH_SWITCH)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_SWITCH
            PEACH_PLATFORM_HANDHELD
            PEACH_ARCH_ARM64
        )

    endif()
endfunction()
