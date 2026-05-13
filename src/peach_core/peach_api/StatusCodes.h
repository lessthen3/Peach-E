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
    typedef enum { //only C23 supports the : int64_t definition which is not great for compatibility
        
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

        //========================================================================== GameManager ==========================================================================//

        PEACH_FATAL_ERROR_FAILED_TO_CREATE_MAIN_WINDOW,

        //=================================================================================================================================================================//

        //======================================================================= Rendering =======================================================================//

        //////////////////////// General Rendering Statuses ////////////////////////

        PEACH_FATAL_FAILED_TO_CREATE_RENDERING_LOGGER,
        PEACH_FATAL_ERROR_PASSED_NULLPTR_REFERENCE_TO_RESOURCE_MANAGER_RENDERING_RESOURCE_PIPE,

        PEACH_WARNING_TRIED_TO_INITIALIZE_RENDERING_MANAGER_AFTER_FULLY_INITIALIZED,

        //////////////////////// OpenGL Specific ////////////////////////

        PEACH_ERROR_FAILED_TO_INITIALIZE_OPENGL,
        PEACH_ERROR_FAILED_INITIALIZE_GLEW,
        PEACH_ERROR_FAILED_TO_CREATE_GL_CONTEXT,

        INTERNAL_PEACH_FATAL_ERROR_PASSED_NULLPTR_REFERENCE_TO_SDL_WINDOW_GL,
        PEACH_INTERNAL_FATAL_ERROR_UNABLE_TO_CREATE_LOGGER_GL,

        //////////////////////// MobileGL ////////////////////////

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_MOBILE_GLES,

        //////////////////////// WebGL ////////////////////////

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_WEB_GL,

        //////////////////////// Vulkan ////////////////////////

        PEACH_ERROR_FAILED_TO_INITIALIZE_VULKAN,
        PEACH_ERROR_FAILED_TO_FIND_VULKAN_DYNAMIC_LIBRARY_FOR_FUNCTION_PFN_QUERYING,

        //////////////////////// Metal ////////////////////////

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_METAL,

        //=========================================================================================================================================================//

        //======================================================================= Physics =======================================================================//

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_PHYSICS_3D,
        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_PHYSICS_2D,


        //=======================================================================================================================================================//

        //======================================================================= Networking =======================================================================//

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_NETWORKING_UWU,

        //==========================================================================================================================================================//

        //======================================================================= Audio =======================================================================//

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_AUDIO_SUBSYSTEM,

        //=================================================================================================================================================================//

        //======================================================================= Resource =======================================================================//

        PEACH_FATAL_ERROR_FAILED_TO_INITIALIZE_RESOURCE_MANAGER,

        PEACH_FATAL_FAILED_TO_INITIALIZE_RENDERING_LOADING_QUEUE

        //=================================================================================================================================================================//

    } PEACH_STATUS_CODE;

#ifdef __cplusplus
}
#endif

#endif /* PEACH_STATUS_CODES_C_H */