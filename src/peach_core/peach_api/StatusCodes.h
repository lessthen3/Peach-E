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


enum PEACH_STATUS_CODE : int64_t
{
    PEACH_OK = 1,

    PEACH_INVALID_SCENE_NAME = -1000,
    PEACH_ERROR_NULLPTR_REF_PASSED,
    PEACH_ERROR_INVALID_NODE_REMOVAL_ID,
    PEACH_ERROR_INVALID_NODE_REMOVAL_NAME

};

//const static unordered_map<int64_t, string> PEACH_STATUS_STRINGS =
//{
//    {PEACH_OK, "Working as expected"},
//    { PEACH_INVALID_SCENE_NAME, "invalid scene name passed, there is no scene with specified name found"}
//};

//constexpr static inline string
//    StatusCodeToString(const uint64_t fp_StatusCode)
//{
//    //auto f_FindIterator = PEACH_STATUS_STRINGS.find(fp_StatusCode);


//    return PEACH_STATUS_STRINGS.at(fp_StatusCode);
//}


#endif