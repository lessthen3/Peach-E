/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#ifndef PEACH_STATUS_CODES_C_H
#define PEACH_STATUS_CODES_C_H

#ifdef __cplusplus 

    #include <stdint.h>

    //#include <cstdint>
    //#include <unordered_map>

#else
    #include <stdint.h>
#endif


typedef enum PEACH_STATUS_CODE //only C23 supports the : int64_t definition which is not great for compatibility
{
    PEACH_OK = 1,

    PEACH_ERROR_INVALID_SCENE_NAME,
    PEACH_ERROR_NULLPTR_REF_PASSED,
    PEACH_ERROR_INVALID_NODE_REMOVAL_ID,
    PEACH_ERROR_INVALID_NODE_REMOVAL_NAME,

    //////////////////////// General Rendering Statuses ////////////////////////

    PEACH_ERROR_FAILED_TO_CREATE_MAIN_WINDOW,

    //////////////////////// OpenGL Specific ////////////////////////

    PEACH_ERROR_FAILED_TO_INITIALIZE_OPENGL,
    PEACH_ERROR_FAILED_INITIALIZE_GLEW,

    //////////////////////// Vulkan ////////////////////////

    PEACH_ERROR_FAILED_TO_INITIALIZE_VULKAN 

};

#endif