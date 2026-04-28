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
#ifndef PEACH_NODE_DEFINITION_API_C_H
#define PEACH_NODE_DEFINITION_API_C_H

#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
    #define PEACH_CONSTEXPR constexpr
#else
    #define PEACH_CONSTEXPR
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum { //fuck the type system when it comes to flags uwu
        PEACH_FLAGS_NONE = 0,

        PEACH_FLAGS_IS_PAUSABLE = (1u << 0),
        PEACH_FLAGS_IS_ACTIVE = (1u << 1),
        PEACH_FLAGS_IS_VISIBLE = (1u << 2),
        PEACH_FLAGS_IS_QUEUED_FOR_REMOVAL = (1u << 3),
        PEACH_FLAGS_IS_INPUT_POLLING_ACTIVE = (1u << 4), // for easily checking if a node script known at export and build time has an input callback function for polling owo
        PEACH_FLAGS_SHOULD_UPDATE_BE_CALLED = (1u << 5) //eg for games driven by user interaction with elements that only react to input
    } PEACH_NodeFlags;

    //XXX: this is used for indexing into the tuple on SceneTree to determine which plf::colony will be accessed so these absolutely need to agree w the c++ internals owo
    typedef enum { // this doesnt really need to be a bit mask tbh idk, future me uwu: fixed it owo
        PEACH_TYPE_RENDER_2D = 0,
        PEACH_TYPE_RENDER_3D = 1u,
        PEACH_TYPE_PHYSICS_2D = 2u,
        PEACH_TYPE_PHYSICS_3D = 3u,
        PEACH_TYPE_AUDIO_2D = 4u,
        PEACH_TYPE_AUDIO_3D = 5u,
        PEACH_TYPE_UTILITY = 6u,
        PEACH_TYPE_INTERFACE = 7u,
        PEACH_TYPE_BLANK = 8u
    } PEACH_NodeType;

    typedef struct{
        uint32_t Index;
        uint32_t Generation;
        PEACH_NodeType Type;
    } PEACH_NodeID;
    
    //small helped if u want owo
    static inline int
        PEACH_IsNodesEqual
        (
            PEACH_NodeID fp_A,
            PEACH_NodeID fp_B
        )
    {
        return fp_A.Index == fp_B.Index && fp_A.Generation == fp_B.Generation && fp_A.Type == fp_B.Type;
    }

#ifdef __cplusplus
}/*__cplusplus*/
#endif

#ifdef __cplusplus

    inline constexpr PEACH_NodeID PEACH_NODE_NULL_ID = { 0, 0, PEACH_TYPE_BLANK }; //Represents no ID, or invalid ID

    // free function operator== for std::find, std::vector comparison etc.
    [[nodiscard]] inline bool
        operator==
    (
        const PEACH_NodeID& fp_A,
        const PEACH_NodeID& fp_B
    )
        noexcept
    {
        return fp_A.Index == fp_B.Index and fp_A.Generation == fp_B.Generation and fp_A.Type == fp_B.Type;
    }

#endif /*__cplusplus*/

#endif /*PEACH_NODE_DEFINITION_API_C_H*/