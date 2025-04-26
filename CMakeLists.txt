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

# Try to find `conan_toolchain.cmake` inside the build directory automatically
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
    "src/Peach-Engine/*.cpp" 
    "include/Peach-Engine/*.h"
)

# Peach Editor
file(
    GLOB_RECURSE 
    PEACH_EDITOR_SOURCES 
    CONFIGURE_DEPENDS 
    "src/Peach-Editor/*.cpp" 
    "include/Peach-Editor/*.h"
)

####################################### Library Target

# Create a static library
add_library(peach STATIC 

    ${PEACH_CORE_SOURCES}
)

# Specify include directories for the library
target_include_directories(peach PUBLIC 

    include/Peach-Core
    "${PROJECT_SOURCE_DIR}/Cool-People/header_only" #header only library includes
)

####################################### Executable Targets

# Peach Engine
add_executable(
    ${PROJECT_NAME} 
    ${PEACH_ENGINE_SOURCES}
)

# Link Peach Engine to Peach Core
target_link_libraries(${PROJECT_NAME} PRIVATE 
    peach
)

# Peach Editor
add_executable( 
    Peach_Editor
    ${PEACH_EDITOR_SOURCES}
)

# Link Peach_Editor to Peach Core
target_link_libraries(Peach_Editor PRIVATE 
    peach
)

####################################### Static Imports

# helper macro for reducing repeated boiler plate for vulkan imports uwu
macro(IMPORTED_LIBRARY TARGET RELEASE_LIB DEBUG_LIB INCLUDE_DIR)
    add_library(${TARGET} STATIC IMPORTED)
    set_target_properties(${TARGET} PROPERTIES
        IMPORTED_LOCATION_DEBUG           ${DEBUG_LIB}
        IMPORTED_LOCATION_RELEASE         ${RELEASE_LIB}
        IMPORTED_LOCATION_RELWITHDEBINFO  ${RELEASE_LIB}
        IMPORTED_LOCATION_MINSIZEREL      ${RELEASE_LIB}
        INTERFACE_INCLUDE_DIRECTORIES     ${INCLUDE_DIR}
    )
endmacro()


if(WIN32 )

    add_library(python313 STATIC IMPORTED)

    set_target_properties(python313 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/python/win64/python313.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/python/include_win64"
    )

    # add_library(GGPO STATIC IMPORTED) #working on GGPO4ALL

    # set_target_properties(GGPO PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    # )

    add_library(SDL3 STATIC IMPORTED)

    set_target_properties(SDL3 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/win64/SDL3-static.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    ############ Vulkan Specific Imports

    IMPORTED_LIBRARY(shaderC 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/shaderc.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/shadercd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    )

    IMPORTED_LIBRARY(volk 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/volk.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/volkd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    )

    ############ SPIRV Tools

    IMPORTED_LIBRARY(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPIRV-Tools.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPIRV-Toolsd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPIRV-Tools-diff.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPIRV-Tools-diffd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPIRV-Tools-link.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPIRV-Tools-linkd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPIRV-Tools-lint.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPIRV-Tools-lintd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPIRV-Tools-opt.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPIRV-Tools-optd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPIRV-Tools-reduce.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPIRV-Tools-reduced.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/SPVRemapper.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/SPVRemapperd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    IMPORTED_LIBRARY(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-core.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-cored.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-cpp.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-cppd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-glsl.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-glsld.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-hlsl.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-hlsld.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-msl.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-msld.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-reflect.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-reflectd.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/win64/spirv-cross-util.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/debug/win64/spirv-cross-utild.lib"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

elseif(APPLE)

    add_library(python313 STATIC IMPORTED)

    set_target_properties(python313 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/python/lib_osxARM64/python313.a"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/python/include_osxARM64"
    )

    # add_library(GGPO STATIC IMPORTED) #THIS IS NOT SUPPORTED ON MAC ATM RIP

    # set_target_properties(GGPO PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    # )

    add_library(SDL3 SHARED IMPORTED)

    set_target_properties(SDL3 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/lib_osxARM64/libSDL3.0.dylib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    ############ Vulkan Specific Imports XXX: doing these later when i compile on my mac mini >w<

    # add_library(moltenVK SHARED IMPORTED)

    # set_target_properties(moltenVK PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_osxARM64/libMoltenVK.dylib"
    # )

    # add_library(shaderC STATIC IMPORTED)

    # set_target_properties(shaderC PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/shaderc.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    # )

    # add_library(volk STATIC IMPORTED)

    # set_target_properties(volk PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/volk.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    # )

    # add_library(SPIRV_Tools STATIC IMPORTED)

    # set_target_properties(SPIRV_Tools PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/SPIRV-Tools.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    # )

    # add_library(SPIRV_Cross STATIC IMPORTED)

    # set_target_properties(SPIRV_Cross PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/spirv-cross-cpp.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    # )

elseif(UNIX) #XXX: doing these later when i compile on my linux machine, future me: WE DID BIIIITCH WE IN IT

    add_library(python313 STATIC IMPORTED)

    set_target_properties(python313 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/python/lib_osxARM64/python313.a"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/python/include_tux64"
    )

    # add_library(GGPO STATIC IMPORTED) #working on GGPO4ALL

    # set_target_properties(GGPO PROPERTIES
    #     IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    #     INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    # )

    add_library(SDL3 STATIC IMPORTED)

    set_target_properties(SDL3 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/tux64/libSDL3.a"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    ############ Vulkan Specific Imports

    IMPORTED_LIBRARY(shaderC 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libshaderc.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    )

    IMPORTED_LIBRARY(volk 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libvolk.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libvolk.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    )

    ############ SPIRV Tools

    IMPORTED_LIBRARY(SPIRV_Tools 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Diff 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-diff.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Link 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-link.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Lint 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-lint.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Opt 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-opt.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Reduce 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPIRV-Tools-reduce.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    IMPORTED_LIBRARY(SPIRV_Tools_Remapper 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libSPVRemapper.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    ############ SPIRV Cross

    IMPORTED_LIBRARY(SPIRV_Cross_Core 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-core.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_CPP 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-cpp.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_GLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-glsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_HLSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-hlsl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_MSL 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-msl.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_Reflect 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-reflect.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
    )

    IMPORTED_LIBRARY(SPIRV_Cross_Util 
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-util.a"
        "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/release/tux64/libspirv-cross-util.a"
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

find_package(pybind11 REQUIRED)
find_package(sol2 REQUIRED)

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
    #lang support
    pybind11::pybind11
    sol2::sol2
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
    #python runtime
    python313 
    #SDL
    SDL3
)

if(APPLE ) #link moltenVK for mac compatibility
    target_link_libraries(peach PUBLIC
        moltenVK
    )
elseif(WIN32)
    target_link_libraries(peach PRIVATE 
        setupapi 
        version 
        imm32 
        winmm 
        gdi32 
        user32 
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
    
    shaderC
)

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
endif()