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
    #try to catch annoying cmake try compile shenanigans
    get_property(f_IsInternalProbe GLOBAL PROPERTY IN_TRY_COMPILE)
    
    if(f_IsInternalProbe OR CMAKE_IN_TRY_COMPILE)
        return() # cmake internal probe, just bail silently uwu
    endif()

    message(FATAL_ERROR 
        "PEACH_TARGET_PLATFORM not specified! daddy needs to know where kitten is running uwu\n"
        "pass -DPEACH_TARGET_PLATFORM=<target> where target is one of:\n"
        " windows, windows-arm64, macos, linux, ios, tvos, android, wasm, psvita, haiku, freebsd"
    )
endif()

############# derive convenience flags from PEACH_TARGET_PLATFORM #############

set(PEACH_WINDOWS_X64       OFF CACHE BOOL "" FORCE)
set(PEACH_WINDOWS_ARM64     OFF CACHE BOOL "" FORCE)

set(PEACH_MACOS_X64         OFF CACHE BOOL "" FORCE)
set(PEACH_MACOS_ARM64       OFF CACHE BOOL "" FORCE)
set(PEACH_IOS               OFF CACHE BOOL "" FORCE)
set(PEACH_TVOS              OFF CACHE BOOL "" FORCE)

set(PEACH_LINUX_X64         OFF CACHE BOOL "" FORCE)
set(PEACH_LINUX_ARM64       OFF CACHE BOOL "" FORCE)

set(PEACH_BSD_X64           OFF CACHE BOOL "" FORCE)
set(PEACH_BSD_ARM64         OFF CACHE BOOL "" FORCE)

set(PEACH_HAIKU             OFF CACHE BOOL "" FORCE)
set(PEACH_ANDROID           OFF CACHE BOOL "" FORCE)
set(PEACH_WASM              OFF CACHE BOOL "" FORCE)
set(PEACH_VITA              OFF CACHE BOOL "" FORCE)
set(PEACH_SWITCH            OFF CACHE BOOL "" FORCE)

set(PEACH_ARCH_X64          OFF CACHE BOOL "" FORCE)
set(PEACH_ARCH_ARM64        OFF CACHE BOOL "" FORCE)     
set(PEACH_ARCH_ARMV7        OFF CACHE BOOL "" FORCE)     

############# category flags #############

set(PEACH_PLATFORM_IS_DESKTOP     OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_MOBILE      OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_HANDHELD    OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_WEB         OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_APPLE       OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_UNIX        OFF CACHE BOOL "" FORCE)

set(PEACH_PLATFORM_IS_WINDOWS     OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_LINUX       OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_FREEBSD     OFF CACHE BOOL "" FORCE)
set(PEACH_PLATFORM_IS_MACOS       OFF CACHE BOOL "" FORCE)


############# windows native #############

if(PEACH_TARGET_PLATFORM STREQUAL "windows-x64")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_WINDOWS_X64            ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP    ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_WINDOWS    ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_X64               ON CACHE BOOL "" FORCE)

    message(STATUS "Mmm, Windows detected, kitten! 😏")

############# windows arm #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "windows-arm64")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_PROCESSOR ARM64)

    set(PEACH_WINDOWS_ARM64       ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_WINDOWS ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

    message(STATUS "Windows ARM64 detected, kitten on a surface pro~ 💅")

############# macos native #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "macos-x64")
    set(CMAKE_SYSTEM_NAME Darwin)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(CMAKE_OSX_ARCHITECTURES x86_64)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")

    set(PEACH_MACOS_X64           ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_APPLE   ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_MACOS   ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_X64            ON CACHE BOOL "" FORCE)

    message(STATUS "macOS x86_64, older but golder ^w^ >///< daddy still loves u kitten owo ~~~🍎")

elseif(PEACH_TARGET_PLATFORM STREQUAL "macos-arm64")
    set(CMAKE_SYSTEM_NAME Darwin)
    set(CMAKE_SYSTEM_PROCESSOR arm64)

    set(CMAKE_OSX_ARCHITECTURES arm64)     #idfk apple clang is stupid needs lowercase LMFAO 
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")

    set(PEACH_MACOS_ARM64         ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_APPLE   ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_MACOS   ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

    message(STATUS "macOS MMMMMMMMMM daddy likey >w<, premium fur and silky frameworks for daddy's kitten~ 🍎")

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

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

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

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

    message(STATUS "tvOS, time to get cozy on the big screen, nya~ 📺")

############# linux native #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "linux-x64")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_LINUX_X64           ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_LINUX   ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_X64            ON CACHE BOOL "" FORCE)

    message(STATUS "Nyaa, Linux detected! Flex that Tux, kitten~ 🐧")

elseif(PEACH_TARGET_PLATFORM STREQUAL "linux-arm64")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR ARM64)

    set(PEACH_LINUX_ARM64         ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_LINUX   ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

    message(STATUS "Linux ARM64 detected — WSL2 in the streets, ARM64 in the sheets 😉") #holy shit opus is gunnin for the gpt crown rn LMFAO

############# freebsd #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "freebsd-x64")
    set(CMAKE_SYSTEM_NAME FreeBSD)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_BSD_X64             ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_FREEBSD ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_X64            ON CACHE BOOL "" FORCE)

    message(STATUS "FreeBSD... true BSD-babe detected, ready for that zfs cuddles~ 🦀")

elseif(PEACH_TARGET_PLATFORM STREQUAL "freebsd-arm64")
    set(CMAKE_SYSTEM_NAME FreeBSD)
    set(CMAKE_SYSTEM_PROCESSOR ARM64)

    set(PEACH_BSD_ARM64           ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_FREEBSD ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

    message(STATUS "FreeBSD ARM64 detected — kitten's all sweaty in this exotic toolchain 💦")

############# haiku #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "haiku")
    set(CMAKE_SYSTEM_NAME Haiku)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)

    set(PEACH_HAIKU               ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_DESKTOP ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_X64            ON CACHE BOOL "" FORCE)
    
    message(STATUS "Haiku OS, lightweight and aesthetic, uwu 💐")

############# android cross #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "android")

    #set abi and versions via init.py

    ############# resolve the NDK path #############
    # init.py is the primary entry point and passes -DCMAKE_ANDROID_NDK=<path> directly, this is just a fall back

    if(NOT DEFINED CMAKE_ANDROID_NDK OR CMAKE_ANDROID_NDK STREQUAL "")
    
        message(STATUS "CMake Android NDK var not set, going to try and find it through peach.toolchain.cmake")

        if(DEFINED ENV{ANDROID_NDK_HOME} AND NOT "$ENV{ANDROID_NDK_HOME}" STREQUAL "")
            set(CMAKE_ANDROID_NDK "$ENV{ANDROID_NDK_HOME}")

        elseif(DEFINED ENV{ANDROID_NDK_ROOT} AND NOT "$ENV{ANDROID_NDK_ROOT}" STREQUAL "")
            set(CMAKE_ANDROID_NDK "$ENV{ANDROID_NDK_ROOT}")

        elseif(DEFINED ENV{ANDROID_NDK} AND NOT "$ENV{ANDROID_NDK}" STREQUAL "")
            set(CMAKE_ANDROID_NDK "$ENV{ANDROID_NDK}")

        else()
            message(FATAL_ERROR
                "Android NDK not found >w< either:\n"
                "  - run the build via init.py which auto-detects, OR\n"
                "  - set ANDROID_NDK_HOME (or ANDROID_NDK_ROOT) env var, OR\n"
                "  - pass -DCMAKE_ANDROID_NDK=<path> directly to cmake"
            )
        endif()
    endif()

    # normalize backslashes to forward slashes regardless of where the path came from (Windows env vars give us C:\foo\bar which CMake's string parser hates)
    file(TO_CMAKE_PATH "${CMAKE_ANDROID_NDK}" CMAKE_ANDROID_NDK)

    if(NOT IS_DIRECTORY "${CMAKE_ANDROID_NDK}")
        message(FATAL_ERROR "CMAKE_ANDROID_NDK is set but doesn't point to a valid directory: '${CMAKE_ANDROID_NDK}'")
    endif()

    message(STATUS "Android NDK found at ${CMAKE_ANDROID_NDK} UwU!!!!!!!!!!!!!!!")

    # Now manually pull in the toolchain
    include("${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")

    set(PEACH_ANDROID             ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_MOBILE  ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_UNIX    ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_ARM64          ON CACHE BOOL "" FORCE)

    message(STATUS "Android detected! Purr in my pocket, take me anywhere~ 🤳🐾")

############# wasm via emscripten #############

elseif(PEACH_TARGET_PLATFORM STREQUAL "wasm")
    set(CMAKE_SYSTEM_NAME Emscripten)
    set(CMAKE_SYSTEM_PROCESSOR wasm32)

    if(NOT DEFINED ENV{EMSDK})
        message(FATAL_ERROR "EMSDK not set! install emscripten from https://emscripten.org and set EMSDK env var uwu")
    endif()

    include("$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake") #grab the emscripten toolchain file uwu

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
    # set(CMAKE_SYSTEM_NAME PSVita)
    # set(CMAKE_SYSTEM_PROCESSOR armv7-a)

    if(NOT DEFINED ENV{VITASDK})
        message(FATAL_ERROR "VITASDK not set! install vitasdk from https://vitasdk.org and set VITASDK env var uwu")
    endif()

    include("$ENV{VITASDK}/share/vita.toolchain.cmake") #set vita sdk toolchain owo
    
    set(PEACH_VITA                  ON CACHE BOOL "" FORCE)
    set(PEACH_PLATFORM_IS_HANDHELD  ON CACHE BOOL "" FORCE)

    set(PEACH_ARCH_ARMV7            ON CACHE BOOL "" FORCE)

    message(STATUS "Kitten on the PS Vita, time to go portable, ahh~ 🎮🍑")

else()
    message(FATAL_ERROR
        "Unknown PEACH_TARGET_PLATFORM: '${PEACH_TARGET_PLATFORM}' >///< \n"
        "valid options: windows-x64, windows-arm64, freebsd-x64, freebsd-arm64, linux-x64, linux-arm64, macos-x64, macos-arm64, ios, tvos, android, wasm, psvita"
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

    if(PEACH_WINDOWS_X64)
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

    elseif(PEACH_MACOS_ARM64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_MACOS
            PEACH_PLATFORM_APPLE
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_ARM64 
        )
    
    elseif(PEACH_MACOS_X64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_MACOS
            PEACH_PLATFORM_APPLE
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64 
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

    elseif(PEACH_LINUX_X64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_LINUX
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64
        )

    elseif(PEACH_LINUX_ARM64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_LINUX
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_ARM64
        )

    elseif(PEACH_BSD_X64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_BSD
            PEACH_PLATFORM_FREEBSD
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_X64
        )

    elseif(PEACH_BSD_ARM64)
        target_compile_definitions(${fp_Target} ${fp_Visibility}
            PEACH_PLATFORM_BSD
            PEACH_PLATFORM_FREEBSD
            PEACH_PLATFORM_DESKTOP
            PEACH_ARCH_ARM64
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
