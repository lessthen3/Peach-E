project(bc7enc_rdo)
cmake_minimum_required(VERSION 3.15)

option(BUILD_X64 "build 64-bit" TRUE)
option(SUPPORT_BC7E "support BC7E (requires ispc)" FALSE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(BC7ENC_SRC_LIST
    bc7enc.cpp
    bc7enc.h
    bc7decomp.cpp
    bc7decomp_ref.cpp
    bc7decomp.h
    rgbcx.cpp
    rgbcx.h
    rgbcx_table4.h
    rgbcx_table4_small.h
    utils.cpp
    utils.h
    miniz.h
    dds_defs.h
    ert.cpp
    ert.h
    rdo_bc_encoder.cpp
    rdo_bc_encoder.h
)

add_library(bc7enc_rdo STATIC ${BC7ENC_SRC_LIST})

target_include_directories(bc7enc_rdo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(bc7enc_rdo PRIVATE /openmp /W4)

        target_compile_options(bc7enc_rdo PRIVATE
        $<$<CONFIG:Debug>:/MTd>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/MT>
    )
else()
    target_compile_options(bc7enc_rdo PRIVATE -fopenmp -fno-strict-aliasing -Wall -Wextra)
    target_link_libraries(bc7enc_rdo PRIVATE m)
endif()