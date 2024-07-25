cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "D:/Game Development/Peach-E/build/build/generators/conan_toolchain.cmake")

include (build/build/generators/conan_toolchain.cmake)

project(Peach_Engine VERSION 1.0 DESCRIPTION "Peach Engine" LANGUAGES CXX)


list(APPEND CMAKE_MODULE_PATH "build/CMakeFiles")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Define directories
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(BINARY_DIR "${PROJECT_BINARY_DIR}/bin")

# Automatically find all the source files in the source directory
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

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

# Add executable based on source files
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
include_directories()

# Link directories
link_directories()

find_package(spdlog CONFIG REQUIRED)
find_package(PhysFS REQUIRED)

find_package(cgal REQUIRED)
find_package(clipper2 CONFIG REQUIRED)

find_package(Bullet CONFIG REQUIRED)

find_package(pybind11 CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Angelscript CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(ZLIB CONFIG REQUIRED)

find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(sfml CONFIG REQUIRED)

find_package(imgui CONFIG REQUIRED)

find_package(dr_libs CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)
find_package(cgltf CONFIG REQUIRED)
find_package(nanosvg CONFIG REQUIRED)


# Linking libraries
target_link_libraries(${PROJECT_NAME} 
    box2d 
    libsodium 
    python312 
    GGPO
    spdlog::spdlog
    physfs-static
    CGAL::CGAL
    Bullet::Bullet
    pybind11::pybind11
    sol2::sol2
    Angelscript::angelscript
    nlohmann_json::nlohmann_json
    cereal::cereal
    ZLIB::ZLIB
    GLEW::GLEW
    glm::glm
    sfml::sfml
    imgui::imgui
    OpenAL::OpenAL
    assimp::assimp)

# Debug and Release specific settings
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINARY_DIR}
)

# Target configurations
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
