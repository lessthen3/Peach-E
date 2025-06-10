####################################### Enforce CMake Version

cmake_minimum_required(VERSION 3.20)

####################################### Project definition

project(Peach_Engine VERSION 1.0 DESCRIPTION "Peach Engine" LANGUAGES CXX)

####################################### Enforcing C++20 standard

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################################### Check For No Config Set

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "No build type specified, ending build. CMAKE WILL NOW EXIT")
endif()

####################################### Enforce 64-bit Requirement

if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    message(FATAL_ERROR "Peach-E only supports 64-bit platforms.")
endif()

####################################### Conan Toolchain Setup

# Try to find conan_toolchain.cmake inside the build directory automatically
file(GLOB_RECURSE CONAN_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/**/conan_toolchain.cmake")

if(EXISTS "${CONAN_TOOLCHAIN_FILE}")
    message(STATUS "Using Conan toolchain: ${CONAN_TOOLCHAIN_FILE}")
    include(${CONAN_TOOLCHAIN_FILE})
else()
    message(FATAL_ERROR "Error: conan_toolchain.cmake not found. Run `conan install` first.")
endif()

####################################### Find All Source Files

# Peach Core
file(
    GLOB_RECURSE 
    PEACH_CORE_SOURCES 
    CONFIGURE_DEPENDS 
    "src/Peach-Core/*.cpp" 
    "include/Peach-Core/*.h"
)

# Peach Engine
file(
    GLOB_RECURSE 
    PEACH_ENGINE_SOURCES 
    CONFIGURE_DEPENDS 
    "src/Peach-Engine/main.cpp" #thin wrapper for Peach Engine executable
)

# Peach Editor
file(
    GLOB_RECURSE 
    PEACH_EDITOR_SOURCES 
    CONFIGURE_DEPENDS 
    "src/Peach-Editor/*.cpp" 
    "include/Peach-Editor/*.h"
)

# Peach API
file(
    GLOB_RECURSE 
    PEACH_API_SOURCES 
    CONFIGURE_DEPENDS 
    "peach_api/PeachAPI.h"
    "peach_api/PeachAPI.cpp"
)

####################################### Static Library Target

# Create a static library
add_library(peach STATIC 

    ${PEACH_CORE_SOURCES}
)

if(WIN32)
    set(PYTHON_INCLUDES "${PROJECT_SOURCE_DIR}/Cool-People/python_includes/win64")

elseif(APPLE)
    set(PYTHON_INCLUDES "${PROJECT_SOURCE_DIR}/Cool-People/python_includes/steveARM64")

elseif(UNIX)
    set(PYTHON_INCLUDES "${PROJECT_SOURCE_DIR}/Cool-People/python_includes/tux64")

else()
    message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH_ENGINE IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX SORRY NOT SORRY")

endif()

# Specify include directories for the library
target_include_directories(peach PUBLIC 

    include/Peach-Core
    "${PROJECT_SOURCE_DIR}/Cool-People/header_only" #header only library includes
    ${PYTHON_INCLUDES}
)

set_target_properties(peach PROPERTIES POSITION_INDEPENDENT_CODE ON) #make peach PIC friendly UwU

####################################### Executable Targets

if(APPLE) #create a bundle instead of a regular exe for macOS

    set(MACOSX_BUNDLE TRUE)

    add_executable(Peach_Editor MACOSX_BUNDLE
        ${PEACH_EDITOR_SOURCES}
    )

    # Peach Editor
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        ${PEACH_ENGINE_SOURCES}
    )

    # Paths to MoltenVK runtime stuff
    set(MOLTENVK_LIB "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libMoltenVK.dylib")
    set(VULKAN_LIB "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libvulkan.1.dylib")
    set(VALIDATION_LAYER "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libVkLayer_khronos_validation.dylib")

    set(JSON_CONFIGS "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/vulkan")

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
    peach
)

# Link Peach_Editor to Peach Core
target_link_libraries(Peach_Editor PRIVATE 
    peach
)

####################################### Dynamic API Library

add_library(peach_api SHARED
    ${PEACH_API_SOURCES}
)

target_link_libraries(peach_api PRIVATE
    peach
)

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


if(WIN32)
    # AND CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"
    # add_library(GGPO STATIC IMPORTED) #working on GGPO4ALL

    # set_target_properties(GGPO PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    # )

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/win64/SDL3-static.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/win64/SDL3-static.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    STATIC_IMPORT(Lua_Compiler 
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/win64/lua/release/lua_compiler.lib"
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/win64/lua/debug/lua_compiler.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/header_only/lua"
    )

    STATIC_IMPORT(nethost 
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/win64/dotnet/libnethost.lib" #juicy static linkage UwU
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/win64/dotnet/nethost.lib" #debug dll
        "${PROJECT_SOURCE_DIR}/Cool-People/header_only/dotnet"
    )

    ############ Vulkan Specific Imports

    STATIC_IMPORT(shaderC 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/shaderc.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/shadercd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    )

    STATIC_IMPORT(volk 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/volk.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/volkd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    )

    ############ SPIRV Tools

    STATIC_IMPORT(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPIRV-Tools.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPIRV-Toolsd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPIRV-Tools-diff.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPIRV-Tools-diffd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPIRV-Tools-link.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPIRV-Tools-linkd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPIRV-Tools-lint.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPIRV-Tools-lintd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPIRV-Tools-opt.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPIRV-Tools-optd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPIRV-Tools-reduce.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPIRV-Tools-reduced.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/SPVRemapper.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/SPVRemapperd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    STATIC_IMPORT(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-core.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-cored.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-cpp.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-cppd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-glsl.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-glsld.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-hlsl.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-hlsld.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-msl.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-msld.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-reflect.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-reflectd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/release/spirv-cross-util.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/win64/debug/spirv-cross-utild.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

elseif(APPLE)

    # add_library(GGPO STATIC IMPORTED) #THIS IS NOT SUPPORTED ON MAC ATM RIP

    # set_target_properties(GGPO PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    # )

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/steveARM64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/steveARM64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    STATIC_IMPORT(Lua_Compiler 
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/steveARM64/lua/liblua_compiler.a"
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/steveARM64/lua/liblua_compiler.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/header_only/lua"
    )

    STATIC_IMPORT(nethost 
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/steveARM64/dotnet/libnethost.a"
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/steveARM64/dotnet/libnethost.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/header_only/dotnet"
    )

    ############ Vulkan Specific Imports

    STATIC_IMPORT(shaderC 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    )

    STATIC_IMPORT(volk 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libvolk.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libvolk.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    )

    ############ SPIRV Tools

    STATIC_IMPORT(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    STATIC_IMPORT(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/steveARM64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

elseif(UNIX) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT
    # AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64"
    # add_library(GGPO STATIC IMPORTED) #working on GGPO4ALL

    # set_target_properties(GGPO PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    # )

    STATIC_IMPORT(SDL3 
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/tux64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/tux64/libSDL3.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    STATIC_IMPORT(Lua_Compiler 
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/tux64/lua/liblua_compiler.a"
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/tux64/lua/liblua_compiler.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/header_only/lua"
    )

    STATIC_IMPORT(nethost 
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/tux64/dotnet/libnethost.a"
        "${PROJECT_SOURCE_DIR}/res/script_runtimes/tux64/dotnet/libnethost.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/header_only/dotnet"
    )

    ############ Vulkan Specific Imports

    STATIC_IMPORT(shaderC 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    )

    STATIC_IMPORT(volk 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libvolk.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libvolk.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    )

    ############ SPIRV Tools

    STATIC_IMPORT(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    STATIC_IMPORT(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    STATIC_IMPORT(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    STATIC_IMPORT(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/tux64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

else()
    message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH_ENGINE IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX SORRY NOT SORRY")

endif()

####################################### Dependency Linking

find_package(PhysFS REQUIRED)

find_package(clipper2 REQUIRED)

find_package(Jolt REQUIRED)
find_package(box2d REQUIRED)

find_package(ZLIB REQUIRED)

find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

find_package(assimp REQUIRED)

find_package(libsodium REQUIRED)

# Peach Core
target_link_libraries(peach PUBLIC
    #file system
    physfs-static
    #geometry manipulation
    clipper2::clipper2
    #physics
    Jolt::Jolt #3D
    box2d::box2d #2D
    #compression
    ZLIB::ZLIB
    #opengl
    GLEW::GLEW
    #cryptography
    libsodium::libsodium
    #asset importing
    assimp::assimp

    #Vulkan
    volk
    glm::glm
    #SDL
    SDL3
)

if(APPLE )

    target_link_libraries(peach PRIVATE
        "-framework AVFoundation"
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreHaptics"
        "-framework CoreVideo"
        "-framework Metal"
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
    target_link_libraries(peach PRIVATE  
        #dont change these, the build breaks for debug otherwise
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:msvcrt>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:advapi32>

        setupapi 
        version 
        imm32 
        winmm 
        kernel32
    ) #needed for statically linking SDL3
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
    Lua_Compiler
    #dotnet
    nethost #used for utilizing the host system's dotnet if it satisfies >=v6.0
)

####################################### Compiler Options and Definitions

target_compile_definitions(peach PRIVATE 
    $<$<CONFIG:Debug>:PEACH_DEBUG>
)

target_compile_definitions(Peach_Editor PRIVATE
    $<$<CONFIG:Debug>:PEACH_DEBUG>
)

target_compile_definitions(peach PUBLIC
    
)

if(WIN32)
    target_compile_definitions(Peach_Editor PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:NETHOST_USE_AS_STATIC>
    )

    target_compile_options(Peach_Editor PRIVATE 
        $<$<CONFIG:Debug>:/MDd> 
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
    )

    target_compile_options(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Debug>:/MDd> 
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
    )

    target_compile_options(peach PRIVATE 
        $<$<CONFIG:Debug>:/MDd> 
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
    )

    target_compile_options(peach_api PRIVATE 
        $<$<CONFIG:Debug>:/MDd> 
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
    )

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
    
    set(NETHOST_DLL_PATH "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/res/script_runtimes/win64/dotnet>")

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_ENVIRONMENT 
        "$<$<CONFIG:Debug>:PATH=${NETHOST_DLL_PATH};%PATH%>"
    )

    set_property(TARGET Peach_Editor PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "")
endif()