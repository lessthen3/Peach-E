cmake_minimum_required(VERSION 3.20)

# Project definition
project(Peach_Engine VERSION 1.0 DESCRIPTION "Peach Engine" LANGUAGES CXX)

# Enforcing C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################################### Conan Setup

# Conan toolchain setup
include (build/build/generators/conan_toolchain.cmake)

####################################### Source Files

# Automatically find all the source files in the source directory
file(
    GLOB_RECURSE 
    SOURCES 
    CONFIGURE_DEPENDS 
    "src/*.cpp" 
    "include/*.h"
)

####################################### Library Target

# Create a static library
add_library(peach STATIC ${SOURCES})

# Specify include directories for the library
target_include_directories(peach PUBLIC 
    include/Peach-Core
)

####################################### Executable Target

# Add executable based on source files
add_executable(${PROJECT_NAME} src/Peach-Editor/main.cpp)

# Link the library with the executable
target_link_libraries(${PROJECT_NAME} PRIVATE peach)

####################################### Static Imports

add_library(box2d STATIC IMPORTED)

set_target_properties(box2d PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/box2d/lib/box2d.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/box2d/include"
)

add_library(python312 STATIC IMPORTED)

set_target_properties(python312 PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/python/lib/python312.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/python/include"
)

add_library(libsodium STATIC IMPORTED)

set_target_properties(libsodium PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/libsodium/lib/libsodium.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/libsodium/include"
)

add_library(GGPO STATIC IMPORTED)

set_target_properties(GGPO PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/lib/GGPO.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/ggpo/include"
)

#
# BGFX STATIC LINKING
#

add_library(bimg STATIC IMPORTED)

set_target_properties(bimg PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/Cool-People/BGFX-stuff/bimg/lib/bimg.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/Cool-People/BGFX-stuff/bimg/include"
)

# Link Statically Compiled External Libraries
target_link_libraries(peach PUBLIC
    box2d 
    libsodium 
    python312 
    GGPO
    bimg
)

####################################### Dependency Linking

find_package(spdlog REQUIRED)
find_package(TBB REQUIRED)
find_package(PhysFS REQUIRED)

find_package(cgal REQUIRED)
find_package(clipper2 REQUIRED)

find_package(Bullet REQUIRED)

find_package(pybind11 REQUIRED)
find_package(sol2 REQUIRED)
find_package(Angelscript REQUIRED)

find_package(nlohmann_json REQUIRED)
find_package(cereal REQUIRED)
find_package(ZLIB REQUIRED)

find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)
find_package(bgfx REQUIRED)

find_package(imgui REQUIRED)

find_package(dr_libs REQUIRED)
find_package(OpenAL REQUIRED)

find_package(assimp REQUIRED)
find_package(cgltf REQUIRED)


# Peach Core
target_link_libraries(peach PUBLIC
    #logging
    spdlog::spdlog
    #threading
    onetbb::onetbb
    #file system
    physfs-static
    #geometry manipulation
    CGAL::CGAL
    clipper2::clipper2
    #3D physics
    Bullet::Bullet
    #lang support
    pybind11::pybind11
    sol2::sol2
    Angelscript::angelscript
    #data handling
    nlohmann_json::nlohmann_json
    cereal::cereal
    ZLIB::ZLIB
    #opengl
    GLEW::GLEW
    glm::glm
    #windowing + rendering
    SDL2::SDL2
    bgfx::bgfx
    imgui::imgui
    #audio
    dr_libs::dr_libs
    OpenAL::OpenAL
)

# Peach Editor
target_link_libraries(${PROJECT_NAME} PUBLIC
    #asset importing
    assimp::assimp
    cgltf::cgltf
)