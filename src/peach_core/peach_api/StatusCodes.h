/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
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
extern "C"
{
#endif
    typedef enum PEACH_STATUS_CODE //only C23 supports the : int64_t definition which is not great for compatibility
    {
        PEACH_OK = 1,
        PEACH_ERROR_INTERNAL_API_FAILURE,

        PEACH_ERROR_NODE_NOT_FOUND,
        PEACH_ERROR_NODE_DOES_NOT_EXIST_IN_TREE,

        PEACH_ERROR_FAILED_TO_INITIALIZE,
        PEACH_ERROR_FAILED_TO_SHUTDOWN_PROPERLY,

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

    } StatusCodes;

#ifdef __cplusplus
}
#endif

#endif /* PEACH_STATUS_CODES_C_H */