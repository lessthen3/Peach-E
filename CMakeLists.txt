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
)

####################################### Executable Targets

# Peach Engine
add_executable(
    ${PROJECT_NAME} 
    ${PEACH_ENGINE_SOURCES}
)

# Link Peach Engine
target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
    peach
)

# Peach Editor
add_executable( 
    Peach_Editor
    ${PEACH_EDITOR_SOURCES}
)

# Link Peach Core to Peach_Editor
target_link_libraries(Peach_Editor PRIVATE 
    peach
)

####################################### Static Imports

if(WIN32 )
    add_library(python313 STATIC IMPORTED)

    set_target_properties(python313 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/python/lib_winx86/python313.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/python/include_winx86"
    )

    add_library(GGPO STATIC IMPORTED)

    set_target_properties(GGPO PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
    )

    add_library(SDL3 STATIC IMPORTED)

    set_target_properties(SDL3 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/lib_winx86/SDL3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/SDL3/include"
    )

    ############ Vulkan Specific Imports

    add_library(vulkan STATIC IMPORTED)

    set_target_properties(vulkan PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/vulkan-1.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/vulkan"
    )

    add_library(shaderC STATIC IMPORTED)

    set_target_properties(shaderC PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/shaderc.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/shaderc"
    )

    add_library(volk STATIC IMPORTED)

    set_target_properties(volk PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/volk.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/Volk"
    )

    add_library(SPIRV_Tools STATIC IMPORTED)

    set_target_properties(SPIRV_Tools PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/SPIRV-Tools.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv-tools"
    )

    add_library(SPIRV_Cross STATIC IMPORTED)

    set_target_properties(SPIRV_Cross PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/lib_winx86/spirv-cross-cpp.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/vulkan/include/spirv_cross"
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

elseif(UNIX)
    message(FATAL_ERROR "nothing for unix/linux just yet oof >w<")

else()
    message(FATAL_ERROR "UNSUPPORTED PLATFORM PEACH_ENGINE IS ONLY SUPPORTED ON WINDOWS/OSX/LINUX SORRY NOT SORRY")

endif()

if(WIN32 )
    # Link Statically Compiled External Libraries
    target_link_libraries(peach PUBLIC
        python313 
        GGPO
        SDL3
        vulkan
        volk
        SPIRV_Cross
    )
else()
    # Link Statically Compiled External Libraries
    target_link_libraries(peach PUBLIC
    python313 
    SDL3
    )
endif()

####################################### Dependency Linking

find_package(PhysFS REQUIRED)

find_package(cgal REQUIRED)
find_package(clipper2 REQUIRED)

find_package(Jolt REQUIRED)
find_package(box2d REQUIRED)

find_package(pybind11 REQUIRED)
find_package(sol2 REQUIRED)

find_package(cereal REQUIRED)
find_package(ZLIB REQUIRED)

find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

find_package(OpenAL REQUIRED)

find_package(assimp REQUIRED)

find_package(libsodium REQUIRED)

# Peach Core
target_link_libraries(peach PUBLIC
    #file system
    physfs-static
    #geometry manipulation
    CGAL::CGAL
    clipper2::clipper2
    #physics
    Jolt::Jolt #3D
    box2d::box2d #2D
    #lang support
    pybind11::pybind11
    sol2::sol2
    #data handling
    cereal::cereal
    ZLIB::ZLIB
    #opengl
    glm::glm
    GLEW::GLEW
    #audio
    OpenAL::OpenAL
    #cryptography
    libsodium::libsodium
    #asset importing
    assimp::assimp
)

# Peach Editor
target_link_libraries(Peach_Editor PUBLIC
    SPIRV_Tools
    shaderC
)

####################################### Set Startup Project (Visual Studio & Xcode)

# Set Peach_Editor as the default scheme in Xcode

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
    set(CMAKE_XCODE_SCHEME_EXECUTABLE Peach_Editor)
    message(STATUS "Setting Peach_Editor as the default startup project for Xcode")
endif()

# Set Peach_Editor as the startup project for Visual Studio

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Peach_Editor)
    message(STATUS "Setting Peach_Editor as the default startup project for VS2022")
endif()