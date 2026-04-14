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
#pragma once

///STL
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring> //for memcpy uwu
#include <filesystem>

#include <type_traits>
#include <utility>

/// fmt
#include <fmt/format.h>

///Peach-E
#include "Logger.h"

/// Magic World

// One field: expands to v.field("a", a) inside peachVisit
#define PEACH_FIELD(f) v(#f, f)

// Base / non-inherited type
#define PEACH_SERIALIZABLE(...)                           \
    using peach_serializable_tag = void;                  \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) {                              \
        __VA_ARGS__;                               \
    }                                                     \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) const {                        \
        __VA_ARGS__;                               \
    }

// Derived type that wants to also serialize its Base part
#define PEACH_SERIALIZABLE_DERIVED(Base, ...)             \
    using peach_serializable_tag = void;                  \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) {                              \
        Base::PEACH_VISIT(v);                              \
        __VA_ARGS__;                               \
    }                                                     \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) const {                        \
        Base::PEACH_VISIT(v);                              \
        __VA_ARGS__;                               \
    }

/*
    idk static error handling with C++ is really fucking annoying when trying to give useful information via an error or failed assert
    found at compile time which is definitely what you want lmfao like wtf man, u introduce constexpr into C++17 but like don't
    consider the fact that static error propogation up a compile time "stack trace" would be useful when doing all this template shit
    at compile time?????????????????????

    like yeah ik that most compilers will implement a proper call stack when the static assert fails but like cmon the compiler fucking
    knows whats wrong why not just let me use it to tell me what it needs u fucks

    annnd yeah ik "oh its coming in C++26", fuck you I don't want to wait another 10 years for gcc, msvc and clang to finally probably
    implement most of the standard fuck you

    although gcc is surpisingly faithful to the standard, idk why I use clang on more posixy platforms lmfao

    well apple-clang is the only way to get things to work properly w all the dynamically linked weirdness and compilation between 
    objc++ <-> C++

    I should just be happy that compiler engineers even allow me to lick their boots, but I'm not >:^(
*/

#define PEACH_STRINGIFY(fp_X) #fp_X
#define PEACH_TOSTRING(fp_X) PEACH_STRINGIFY(fp_X)

#define PEACH_TO_JSON(fp_DesiredObject, fp_DesiredFileName, fp_DesiredOutputDirectory, fp_Logger) \
    ( \
        []() consteval \
         { \
            static_assert \
            ( \
                ::PeachCore::Serializer::is_serializable_struct<remove_cvref_t<decltype(fp_DesiredObject)>>::value, \
                "PEACH_TO_JSON at " __FILE__ " ln " PEACH_TOSTRING(__LINE__) ", T is missing PEACH_SERIALIZABLE(...), did you add the macro to your type uwu?" \
            ); \
                return 0; \
        }(), \
        ::PeachCore::Serializer::ToJSON(fp_DesiredObject, fp_DesiredFileName, fp_DesiredOutputDirectory, fp_Logger) \
    )

#define PEACH_FROM_JSON(fp_DesiredObject, fp_FilePath, fp_Logger) \
    ( \
        []() consteval \
         { \
            static_assert \
            ( \
                ::PeachCore::Serializer::is_serializable_struct<remove_cvref_t<decltype(fp_DesiredObject)>>::value, \
                "PEACH_FROM_JSON at " __FILE__ " ln " PEACH_TOSTRING(__LINE__) ", T is missing PEACH_SERIALIZABLE(...), did you add the macro to your type uwu?" \
            ); \
                return 0; \
        }(), \
        ::PeachCore::Serializer::FromJSON(fp_DesiredObject, fp_FilePath, fp_Logger) \
    )

#define PEACH_PACK_BINARY(fp_DesiredObject, fp_BinaryVector) \
    ( \
        []() consteval \
         { \
            static_assert \
            ( \
                ::PeachCore::Serializer::is_serializable_struct<remove_cvref_t<decltype(fp_DesiredObject)>>::value, \
                "PEACH_PACK_BINARY at " __FILE__ " ln " PEACH_TOSTRING(__LINE__) ", T is missing PEACH_SERIALIZABLE(...), did you add the macro to your type uwu?" \
            ); \
                return 0; \
        }(), \
        ::PeachCore::Serializer::PackIntoBinaryVector(fp_DesiredObject, fp_BinaryVector) \
    )

#define PEACH_UNPACK_BINARY(fp_EmptyObject, fp_BinaryVector, fp_Logger) \
    ( \
        []() consteval \
         { \
            static_assert \
            ( \
                ::PeachCore::Serializer::is_serializable_struct<remove_cvref_t<decltype(fp_EmptyObject)>>::value, \
                "PEACH_UNPACK_BINARY at " __FILE__ " ln " PEACH_TOSTRING(__LINE__) ", T is missing PEACH_SERIALIZABLE(...), did you add the macro to your type uwu?" \
            ); \
                return 0; \
        }(), \
        ::PeachCore::Serializer::UnpackFromBinaryVector(fp_EmptyObject, fp_BinaryVector, fp_Logger) \
    )

#define PEACH_UNPACK_BINARY_OFFSET(fp_EmptyObject, fp_BinaryVector, fp_Logger, fp_StartReadOffset) \
    ( \
        []() consteval \
         { \
            static_assert \
            ( \
                ::PeachCore::Serializer::is_serializable_struct<remove_cvref_t<decltype(fp_EmptyObject)>>::value, \
                "PEACH_UNPACK_BINARY_OFFSET at " __FILE__ " ln " PEACH_TOSTRING(__LINE__) ", T is missing PEACH_SERIALIZABLE(...), did you add the macro to your type uwu?" \
            ); \
                return 0; \
        }(), \
        ::PeachCore::Serializer::UnpackFromBinaryVector(fp_EmptyObject, fp_BinaryVector, fp_Logger, fp_StartReadOffset) \
    )


template<typename>
inline constexpr bool always_false_v = false;

/// back to normality >W<

namespace PeachCore {

    constexpr size_t MAX_ARRAY_LINE_WIDTH = 20;

    namespace BinaryCodec { //all these functions use big-endian

        //////////////////////////////////////////////
        // Encoding Functions
        //////////////////////////////////////////////

        inline void
            EncodeUint8
            (
                vector<uint8_t>& fp_Bytecode,
                const uint8_t fp_Int
            )
        {
            fp_Bytecode.push_back(fp_Int);
        }

        inline void
            EncodeUint16
            (
                vector<uint8_t>& fp_ByteCode,
                const uint16_t fp_Int
            )
        {
            fp_ByteCode.push_back((fp_Int >> 8) & 0xFF); // High byte
            fp_ByteCode.push_back(fp_Int & 0xFF);         // Low byte
        }

        inline void
            EncodeUint32
            (
                vector<uint8_t>& fp_ByteCode,
                const uint32_t fp_Int
            )
        {
            fp_ByteCode.push_back((fp_Int >> 24) & 0xFF); // High byte
            fp_ByteCode.push_back((fp_Int >> 16) & 0xFF);
            fp_ByteCode.push_back((fp_Int >> 8) & 0xFF);
            fp_ByteCode.push_back(fp_Int & 0xFF);         // Low byte
        }

        inline void
            EncodeUint64
            (
                vector<uint8_t>& fp_ByteCode,
                const uint64_t fp_Int
            )
        {
            fp_ByteCode.push_back((fp_Int >> 56) & 0xFF); // High byte
            fp_ByteCode.push_back((fp_Int >> 48) & 0xFF);
            fp_ByteCode.push_back((fp_Int >> 40) & 0xFF);
            fp_ByteCode.push_back((fp_Int >> 32) & 0xFF);
            fp_ByteCode.push_back((fp_Int >> 24) & 0xFF);
            fp_ByteCode.push_back((fp_Int >> 16) & 0xFF);
            fp_ByteCode.push_back((fp_Int >> 8) & 0xFF);
            fp_ByteCode.push_back(fp_Int & 0xFF);         // Low byte
        }

        inline void
            EncodeInt8
            (
                vector<uint8_t>& fp_Bytecode,
                const int8_t fp_Int
            )
        {
            EncodeUint8(fp_Bytecode, static_cast<uint8_t>(fp_Int));
        }

        inline void
            EncodeInt16
            (
                vector<uint8_t>& fp_ByteCode,
                const int16_t fp_Int
            )
        {
            EncodeUint16(fp_ByteCode, static_cast<uint16_t>(fp_Int));
        }

        inline void
            EncodeInt32
            (
                vector<uint8_t>& fp_ByteCode,
                const int32_t fp_Int
            )
        {
            EncodeUint32(fp_ByteCode, static_cast<uint32_t>(fp_Int));
        }

        inline void
            EncodeInt64
            (
                vector<uint8_t>& fp_ByteCode,
                const int64_t fp_Int
            )
        {
            EncodeUint64(fp_ByteCode, static_cast<uint64_t>(fp_Int));
        }

        template <typename T>
        inline void 
            EncodeInt
            (
                vector<uint8_t>& fp_Bytecode, 
                const T fp_Int
            )
        {
            static_assert
            (
                is_same_v<T, uint8_t> or is_same_v<T, uint16_t> or is_same_v<T, uint32_t> or is_same_v<T, uint64_t>
                or is_same_v<T, int8_t> or is_same_v<T, int16_t> or is_same_v<T, int32_t> or is_same_v<T, int64_t>,
                "EncodeInt only accepts 8/16/32/64-bit signed or unsigned integer types"
            );

            if constexpr (sizeof(T) == 1)
            {
                EncodeUint8(fp_Bytecode, static_cast<uint8_t>(fp_Int));
            }
            else if constexpr (sizeof(T) == 2)
            {
                EncodeUint16(fp_Bytecode, static_cast<uint16_t>(fp_Int));
            }
            else if constexpr (sizeof(T) == 4)
            {
                EncodeUint32(fp_Bytecode, static_cast<uint32_t>(fp_Int));
            }
            else if constexpr (sizeof(T) == 8)
            {
                EncodeUint64(fp_Bytecode, static_cast<uint64_t>(fp_Int));
            }
            else
            {
                static_assert(always_false_v<T>, "EncodeInt only supports 8/16/32/64-bit integer types");
            }
        }

        inline void
            EncodeBool
            (
                vector<uint8_t>& fp_ByteCode, 
                bool fp_Bool
            )
        {
            uint8_t f_BoolAsInt = fp_Bool ? 1 : 0; // Convert boolean to 32-bit integer
            fp_ByteCode.push_back(f_BoolAsInt);
        }

        inline void
            EncodeFloat
            (
                vector<uint8_t>& fp_ByteCode,
                const float fp_Float
            )
        {
            uint32_t f_AsInt;
            memcpy(&f_AsInt, &fp_Float, sizeof(float)); // Copy the float into an uint32_t bit pattern
            EncodeUint32(fp_ByteCode, f_AsInt);      // Reuse the integer encoding function
        }

        inline void
            EncodeDouble
            (
                vector<uint8_t>& fp_ByteCode,
                const double fp_DoubleVal
            )
        {
            uint64_t f_AsInt;
            memcpy(&f_AsInt, &fp_DoubleVal, sizeof(double)); // Copy the double into a uint64_t bit pattern
            EncodeUint64(fp_ByteCode, f_AsInt);      // Reuse the integer encoding function
        }

        template<typename LengthT>
        inline void 
            EncodeStringWithoutEscapeCharacters
            (
                vector<uint8_t>& fp_ByteCode,
                const string& fp_String
            )
        {
            ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

            static_assert
            (
                is_same_v<LengthT, uint8_t> or is_same_v<LengthT, uint16_t> or is_same_v<LengthT, uint32_t> or is_same_v<LengthT, uint64_t>,
                "EncodeStringWithoutEscapeCharacters only accepts 8/16/32/64-bit unsigned integer types"
            );

            ////////////////////////////////////////////// Overflow Check on String Size With Passed Type //////////////////////////////////////////////

            if (fp_String.size() > numeric_limits<LengthT>::max())
            {
                throw length_error("EncodeStringWithoutEscapeCharacters: string too long for LengthT");
            }

            ////////////////////////////////////////////// Encode String Length //////////////////////////////////////////////

            EncodeInt<LengthT>(fp_ByteCode, static_cast<LengthT>(fp_String.size()));

            ////////////////////////////////////////////// Write Character by Character //////////////////////////////////////////////

            for (const char lv_Char : fp_String)
            {
                fp_ByteCode.push_back(static_cast<uint8_t>(lv_Char));
            }
        }

        template<typename LengthT>
        inline void
            EncodeStringUTF8
            (
                vector<uint8_t>& fp_ByteCode,
                const string& fp_String
            )
        {
            ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

            static_assert
            (
                is_same_v<LengthT, uint8_t> or is_same_v<LengthT, uint16_t> or is_same_v<LengthT, uint32_t> or is_same_v<LengthT, uint64_t>,
                "EncodeStringUTF8 only accepts 8/16/32/64-bit unsigned integer types"
            );

            ////////////////////////////////////////////// Create Temporary Buffer //////////////////////////////////////////////

            vector<uint8_t> f_TempBuffer; //use a temp buffer so that size can be encoded first, and escape characters need to be double counted
            f_TempBuffer.reserve(fp_String.size());

            ////////////////////////////////////////////// Write Character by Character //////////////////////////////////////////////

            for (const char lv_Char : fp_String)
            {
                switch (lv_Char)
                {
                case '\n':  // Newline
                    f_TempBuffer.push_back('\\');
                    f_TempBuffer.push_back('n');
                    break;
                case '\t':  // Tab
                    f_TempBuffer.push_back('\\');
                    f_TempBuffer.push_back('t');
                    break;
                case '\\':  // Backslash
                    f_TempBuffer.push_back('\\');
                    f_TempBuffer.push_back('\\');
                    break;
                default:
                    f_TempBuffer.push_back(static_cast<uint8_t>(lv_Char));
                    break;
                }
            }

            ////////////////////////////////////////////// Overflow Check on String Size With Passed Type //////////////////////////////////////////////

            if (f_TempBuffer.size() > numeric_limits<LengthT>::max())
            {
                throw length_error("EncodeStringUTF8: string too long for given type");
            }

            ////////////////////////////////////////////// Encode String Length //////////////////////////////////////////////

            EncodeInt<LengthT>(fp_ByteCode, static_cast<LengthT>(f_TempBuffer.size()));

            ////////////////////////////////////////////// Append Temp Buffer -> Bytecode //////////////////////////////////////////////

            fp_ByteCode.insert(fp_ByteCode.end(), f_TempBuffer.begin(), f_TempBuffer.end());
        }

        //////////////////////////////////////////////
        // Decoding Functions
        //////////////////////////////////////////////

        [[nodiscard]] inline uint8_t
            DecodeUint8
            (
                const vector<uint8_t>& fp_Bytecode,
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if (fp_Offset >= fp_Bytecode.size())
            {
                throw runtime_error("DecodeUint8: out of bounds");
            }

            ////////////////////////////////////////////// Decode 8-Bit Int //////////////////////////////////////////////

            return fp_Bytecode[fp_Offset++];
        }

        [[nodiscard]] inline uint16_t
            DecodeUint16
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if (fp_Offset + 1 >= fp_ByteCode.size())
            {
                throw runtime_error("DecodeUint16: out of bounds");
            }

            ////////////////////////////////////////////// Decode 16-Bit Int //////////////////////////////////////////////

            uint16_t f_Value =
                (static_cast<uint16_t>(fp_ByteCode[fp_Offset]) << 8) |
                (static_cast<uint16_t>(fp_ByteCode[fp_Offset + 1]));

            ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

            fp_Offset += 2; // End on fresh byte right after the last decoded byte uwu

            ////////////////////////////////////////////// Return Decoded 16-Bit Int //////////////////////////////////////////////

            return f_Value;
        }

        [[nodiscard]] inline uint32_t
            DecodeUint32
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if (fp_Offset + 3 >= fp_ByteCode.size())
            {
                throw runtime_error("DecodeUint32: out of bounds");
            }

            ////////////////////////////////////////////// Decode 32-Bit Int //////////////////////////////////////////////

            uint32_t f_Value =
                (static_cast<uint32_t>(fp_ByteCode[fp_Offset]) << 24) |
                (static_cast<uint32_t>(fp_ByteCode[fp_Offset + 1]) << 16) |
                (static_cast<uint32_t>(fp_ByteCode[fp_Offset + 2]) << 8) |
                (static_cast<uint32_t>(fp_ByteCode[fp_Offset + 3]));

            ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

            fp_Offset += 4; // End on fresh byte right after the last decoded byte uwu

            ////////////////////////////////////////////// Return Decoded 32-Bit Int //////////////////////////////////////////////

            return f_Value;
        }

        [[nodiscard]] inline uint64_t
            DecodeUint64
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if (fp_Offset + 7 >= fp_ByteCode.size())
            {
                throw runtime_error("DecodeUint64: out of bounds");
            }

            ////////////////////////////////////////////// Decode 64-Bit Int //////////////////////////////////////////////

            uint64_t f_Value =
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset]) << 56) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 1]) << 48) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 2]) << 40) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 3]) << 32) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 4]) << 24) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 5]) << 16) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 6]) << 8) |
                (static_cast<uint64_t>(fp_ByteCode[fp_Offset + 7]));

            ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

            fp_Offset += 8; // End on fresh byte right after the last decoded byte uwu

            ////////////////////////////////////////////// Return Decoded 64-Bit Int //////////////////////////////////////////////

            return f_Value;
        }

        [[nodiscard]] inline int8_t
            DecodeInt8
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            return static_cast<int8_t>(DecodeUint8(fp_ByteCode, fp_Offset));
        }

        [[nodiscard]] inline int16_t
            DecodeInt16
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            return static_cast<int16_t>(DecodeUint16(fp_ByteCode, fp_Offset));
        }

        [[nodiscard]] inline int32_t
            DecodeInt32
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            return static_cast<int32_t>(DecodeUint32(fp_ByteCode, fp_Offset));
        }

        [[nodiscard]] inline int64_t
            DecodeInt64
            (
                const vector<uint8_t>& fp_ByteCode,
                size_t& fp_Offset
            )
        {
            return static_cast<int64_t>(DecodeUint64(fp_ByteCode, fp_Offset));
        }

        template <typename T>
        [[nodiscard]] inline T
            DecodeInt
            (
                const vector<uint8_t>& fp_Bytecode, 
                size_t& fp_Offset
            )
        {
            static_assert
            (
                is_same_v<T, uint8_t> or is_same_v<T, uint16_t> or is_same_v<T, uint32_t> or is_same_v<T, uint64_t>
                or is_same_v<T, int8_t> or is_same_v<T, int16_t> or is_same_v<T, int32_t> or is_same_v<T, int64_t>,
                "DecodeInt only accepts integer types, you tried to pass a non integer type"
            );
            
            if constexpr (sizeof(T) == 1)
            {
                return static_cast<T>(DecodeUint8(fp_Bytecode, fp_Offset));
            }
            else if constexpr (sizeof(T) == 2)
            {
                return static_cast<T>(DecodeUint16(fp_Bytecode, fp_Offset));
            }
            else if constexpr (sizeof(T) == 4)
            {
                return static_cast<T>(DecodeUint32(fp_Bytecode, fp_Offset));
            }
            else if constexpr (sizeof(T) == 8)
            {
                return static_cast<T>(DecodeUint64(fp_Bytecode, fp_Offset));
            }
            else
            {
                static_assert(always_false_v<T>, "DecodeInt only supports 8/16/32/64-bit integer types");
            }
        }

        [[nodiscard]] inline bool
            DecodeBool
            (
                const vector<uint8_t>& fp_Bytecode,
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if (fp_Offset >= fp_Bytecode.size())
            {
                throw runtime_error("DecodeBool: out of bounds");
            }

            ////////////////////////////////////////////// Coerce uint8_t -> bool and Increment Offset by 1 Byte //////////////////////////////////////////////

            return fp_Bytecode[fp_Offset++] != 0; //revolutionary uwu
        }

        template<typename LengthT>
        [[nodiscard]] inline string
            DecodeStringWithoutEscapeCharacters
            (
                const vector<uint8_t>& fp_Bytecode, 
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

            static_assert
            (
                is_same_v<LengthT, uint8_t> or is_same_v<LengthT, uint16_t> or is_same_v<LengthT, uint32_t> or is_same_v<LengthT, uint64_t>,
                "DecodeStringWithoutEscapeCharacters only accepts 8/16/32/64-bit unsigned integer types"
            );

            ////////////////////////////////////////////// Get String Length //////////////////////////////////////////////

            size_t f_StringLength = DecodeInt<LengthT>(fp_Bytecode, fp_Offset);

            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if(fp_Offset + f_StringLength > fp_Bytecode.size()) // not >= because lv_CurrentOffset -> f_StringLength - 1, so if fp_Offset + f_StringLength == size() it's fine
            {
                throw runtime_error("DecodeStringWithoutEscapeCharacters: out of bounds");
            }

            ////////////////////////////////////////////// Create String and Allocate Space //////////////////////////////////////////////

            string f_TempString;
            f_TempString.reserve(f_StringLength); // Reserve space to optimize append operations and avoid pointer invalidation and chunk resizing overhead

            ////////////////////////////////////////////// Decode String //////////////////////////////////////////////

            for (size_t lv_CurrentOffset = 0; lv_CurrentOffset < f_StringLength; ++lv_CurrentOffset)
            {
                f_TempString.push_back(static_cast<char>(fp_Bytecode[fp_Offset + lv_CurrentOffset]));
            }

            ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

            fp_Offset += f_StringLength; // End on fresh byte right after the last decoded byte uwu

            ////////////////////////////////////////////// Return Decoded String //////////////////////////////////////////////

            return f_TempString;
        }

        template<typename LengthT>
        [[nodiscard]] string
            DecodeStringUTF8
            (
                const vector<uint8_t>& fp_Bytecode,
                size_t& fp_Offset
            )
        {
            ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

            static_assert
            (
                is_same_v<LengthT, uint8_t> or is_same_v<LengthT, uint16_t> or is_same_v<LengthT, uint32_t> or is_same_v<LengthT, uint64_t>,
                "DecodeStringUTF8 only accepts 8/16/32/64-bit unsigned integer types"
            );

            ////////////////////////////////////////////// Get String Length //////////////////////////////////////////////

            size_t f_StringLength = DecodeInt<LengthT>(fp_Bytecode, fp_Offset);
            size_t f_EndRegionIndex = fp_Offset + f_StringLength;

            ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

            if (fp_Offset + f_StringLength > fp_Bytecode.size()) // not >= because lv_CurrentOffset -> f_StringLength - 1, so if fp_Offset + f_StringLength == size() it's fine
            {
                throw runtime_error("DecodeStringUTF8: out of bounds");
            }

            ////////////////////////////////////////////// Create String and Allocate Space //////////////////////////////////////////////

            string f_DecodedString;
            f_DecodedString.reserve(f_StringLength); // Reserve space to optimize append operations

            ////////////////////////////////////////////// Decode String //////////////////////////////////////////////

            for (size_t lv_CurrentOffset = 0; lv_CurrentOffset < f_StringLength; ++lv_CurrentOffset)
            {
                size_t f_CurrentIndex = fp_Offset + lv_CurrentOffset;
                char f_CurrentChar = static_cast<char>(fp_Bytecode[f_CurrentIndex]);

                if (f_CurrentChar == '\\' and f_CurrentIndex + 1 < f_EndRegionIndex) // Check for escape character and ensure it's not the last char
                {
                    char f_NextChar = static_cast<char>(fp_Bytecode[f_CurrentIndex + 1]);

                    switch (f_NextChar)
                    {
                    case 'n':
                        f_DecodedString.push_back('\n');
                        lv_CurrentOffset++;  // Skip the 'n' character in the stream
                        break;
                    case 't':
                        f_DecodedString.push_back('\t');
                        lv_CurrentOffset++;  // Skip the 't' character in the stream
                        break;
                    case '\\':
                        f_DecodedString.push_back('\\');
                        lv_CurrentOffset++;  // Skip the next '\'
                        break;
                    default:
                        f_DecodedString.push_back(f_CurrentChar);  // If it's not a recognized escape sequence, add the backslash
                        break;
                    }
                }
                else
                {
                    f_DecodedString.push_back(f_CurrentChar);
                }
            }

            ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

            fp_Offset += f_StringLength; // End on fresh byte right after the last decoded byte uwu

            ////////////////////////////////////////////// Return Decoded String //////////////////////////////////////////////

            return f_DecodedString;
        }

        [[nodiscard]] inline float
            DecodeFloat
            (
                const vector<uint8_t>& fp_ByteCode, 
                size_t& fp_Offset
            )
        {
            uint32_t f_AsInt = DecodeUint32(fp_ByteCode, fp_Offset); //decode as an uint since we can just memcpy the bits into a float
            float f_Val;
            memcpy(&f_Val, &f_AsInt, sizeof(float)); // Copy the bits into a float

            return f_Val;
        }

        [[nodiscard]] inline double
            DecodeDouble
            (
                const vector<uint8_t>& fp_ByteCode, 
                size_t& fp_Offset
            )
        {
            uint64_t f_AsInt = DecodeUint64(fp_ByteCode, fp_Offset); //decode as an uint since we only need the bits
            double f_Val;
            memcpy(&f_Val, &f_AsInt, sizeof(double)); // Copy the bits into a float

            return f_Val;
        }
    }//namespace BinaryCodec

    namespace FileIO
    {
        //////////////////////////////////////////////
        // Binary File Read/Write Functions
        //////////////////////////////////////////////

        [[nodiscard]] inline bool
            WriteToBinary
            (
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredName,
                const vector<uint8_t>& fp_Binary,
                Logger*const logger
            )
        {
            ////////////////////////////////////////////// Logger nullptr Safety Check //////////////////////////////////////////////

            if (not logger)
            {
                PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during WriteToBinary()");
                return false;
            }

            ////////////////////////////////////////////// Ensure Directory Exists //////////////////////////////////////////////

            if (not filesystem::exists(fp_DesiredOutputDirectory))
            {
                logger->Error(fmt::format("Serialization Error: Tried to pass invalid write directory: '{}' to WriteToBinary", fp_DesiredOutputDirectory), "Serializer");
                return false;
            }

            ////////////////////////////////////////////// Make Sure fp_Binary is Not Empty //////////////////////////////////////////////

            if (fp_Binary.empty()) //check if the byte vector is empty uwu
            {
                logger->Error(fmt::format("Serialization Error: Tried passing empty byte vector for writing to file name: '{}', nothing was done.", fp_DesiredName), "Serializer");
                return false;
            }

            string f_FileName;

            if (fp_DesiredOutputDirectory == "./")
            {
                f_FileName = "./" + fp_DesiredName;
            }
            else
            {
                f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName;
            }

            ofstream file(f_FileName, ios::binary);  // Open in regular string mode

            if (not file)
            {
                logger->Error(fmt::format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer");
                return false;
            }

            // Write the entire contents of the vector -> peach binary
            file.write(reinterpret_cast<const char*>(fp_Binary.data()), fp_Binary.size());
            // Close the file
            file.close();

            return true; //success! wrote byte vector -> peach binary
        }

        [[nodiscard]] inline bool
            ReadBinaryIntoVector //i think this'll work lmfao
            (
                const string& fp_ScriptFilePath,
                const vector<string>& fp_Extensions,
                vector<uint8_t>& fp_Binary,
                Logger*const logger
            )
        {
            ////////////////////////////////////////////// Logger nullptr Safety Check //////////////////////////////////////////////

            if (not logger) //check for nullptr ref passed to ReadBinaryIntoVector
            {
                PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during ReadBinaryIntoVector()");
                return false;
            }

            // Ensure directory exists
            if (not filesystem::exists(fp_ScriptFilePath))
            {
                logger->Error("Serialization Error: Tried to pass invalid directory to ReadBinaryIntoVector()", "Serializer");
                return false;
            }

            if (not fp_Binary.empty()) //check if the byte vector is empty before reading data into it OwO
            {
                logger->Error(fmt::format("Serialization Error: Tried passing non-empty byte vector for reading to file name: '{}', nothing was done.", fp_ScriptFilePath), "Serializer");
                return false;
            }

            // Extract file extension assuming fmt::format "filename.ext"
            size_t f_LastDotIndex = fp_ScriptFilePath.rfind('.');

            if (f_LastDotIndex == string::npos)
            {
                logger->Error("Serialization Error: No file extension found for Peach-E Binary", "Serializer");
                return false;
            }

            string f_FileExtension = fp_ScriptFilePath.substr(f_LastDotIndex);

            bool f_IsValidExtension = false;

            for (const string& lv_ExtensionName : fp_Extensions)
            {
                if (f_FileExtension == lv_ExtensionName) //file extension for peach-e binary encoding, get it? it's like a bin of peaches >w<
                {
                    f_IsValidExtension = true;
                    break;
                }
            }

            if (not f_IsValidExtension)
            {
                logger->Error(fmt::format("Serialization Error: Attempted to read from an unknown binary extension: '{}'", f_FileExtension), "Serializer");
                return false;
            }

            ifstream f_BinaryStream(fp_ScriptFilePath, ios::binary); //open in binary mode

            if (not f_BinaryStream) //check if the file opened properly
            {
                logger->Error(fmt::format("Serialization Error: Failed to open binary for reading: '{}'", fp_ScriptFilePath), "Serializer");
                return false;
            }

            // Get the size of the file
            f_BinaryStream.seekg(0, ios::end);
            size_t f_Size = f_BinaryStream.tellg();
            f_BinaryStream.seekg(0, ios::beg);

            // Resize the vector to the size of the file
            fp_Binary.resize(f_Size);

            // Read the entire file into the vector
            f_BinaryStream.read(reinterpret_cast<char*>(fp_Binary.data()), f_Size);
            // Close the file
            f_BinaryStream.close();

            return true; //success! file read into vector
        }

        //////////////////////////////////////////////
        // JSON File Read/Write Functions
        //////////////////////////////////////////////

        [[nodiscard]] inline bool
            WriteStringToFile
            (
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredName,
                const string& fp_FileString,
                Logger*const logger
            )
        {
            if (not logger)
            {
                PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during WriteToJSON()");
                return false;
            }

            // Ensure directory exists
            if (not filesystem::exists(fp_DesiredOutputDirectory))
            {
                logger->Error("Serialization Error: Tried to pass invalid write directory to WriteToJSON", "Serializer");
                return false;
            }

            string f_FileName;

            if (fp_DesiredOutputDirectory == "./")
            {
                f_FileName = "./" + fp_DesiredName;
            }
            else
            {
                f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName;
            }

            ofstream f_OpenedFile(f_FileName, ios::out);  // Open in regular string mode

            if (not f_OpenedFile)
            {
                logger->Error(fmt::format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer");
                return false;
            }

            // Write JSON string -> .json file
            f_OpenedFile.write(fp_FileString.c_str(), fp_FileString.size());
            // Close the file
            f_OpenedFile.close();

            return true;
        }

        [[nodiscard]] inline bool
            ReadFileIntoCharBuffer
            (
                const string& fp_ScriptFilePath,
                const vector<string>& fp_Extensions,
                vector<char>& fp_CharBuffer,
                Logger*const logger
            )
        {
            ////////////////////////////////////////////// Logger nullptr Safety Check //////////////////////////////////////////////

            if (not logger)
            {
                PRINT_ERROR("Serialization Error: Tried to pass nullptr reference to logger during ReadFileIntoCharBuffer()");
                return false;
            }

            ////////////////////////////////////////////// Ensure directory exists //////////////////////////////////////////////

            if (not filesystem::exists(fp_ScriptFilePath))
            {
                logger->Error("Serialization Error: Tried to pass invalid filepath to ReadFileIntoCharBuffer()", "Serializer");
                return false;
            }

            ////////////////////////////////////////////// Extract file extension assuming fmt::format "filename.ext" //////////////////////////////////////////////

            size_t lastDotIndex = fp_ScriptFilePath.rfind('.');

            if (lastDotIndex == string::npos)
            {
                logger->Error("Serialization Error: No file extension found", "Serializer");
                return false;
            }

            string f_FileExtension = fp_ScriptFilePath.substr(lastDotIndex);

            bool f_IsValidExtension = false;

            for (const string& lv_ExtensionName : fp_Extensions)
            {
                if (f_FileExtension == lv_ExtensionName) //file extension for peach-e binary encoding, get it? it's like a bin of peaches >w<
                {
                    f_IsValidExtension = true;
                    break;
                }
            }

            if (not f_IsValidExtension)
            {
                logger->Error(fmt::format("Serialization Error: Attempted to read from an unknown text file extension: '{}'", f_FileExtension), "Serializer");
                return false;
            }

            ifstream f_FileStream(fp_ScriptFilePath, ios::in | ios::binary);

            if (not f_FileStream)
            {
                logger->Error(fmt::format("Serialization Error: Failed to open '{}' for reading.", fp_ScriptFilePath), "Serializer");
                return false;
            }

            ////////////////////////////////////////////// Get File Size //////////////////////////////////////////////

            f_FileStream.seekg(0, ios::end);
            streampos f_FileSize = f_FileStream.tellg();
            f_FileStream.seekg(0, ios::beg);

            ////////////////////////////////////////////// Assert File Contains Data //////////////////////////////////////////////

            if (f_FileSize <= 0) //Treat empty files as an error since user thinks the file has something otherwise they wouldn't have tried to read from it UwU!
            {
                logger->Error("Serialization Error: Tried to pass empty file to ReadJSONIntoString()", "Serializer");
                fp_CharBuffer.clear();
                return false;
            }

            ////////////////////////////////////////////// Store Data -> fp_CharBuffer //////////////////////////////////////////////

            fp_CharBuffer.resize(static_cast<size_t>(f_FileSize));
            f_FileStream.read(fp_CharBuffer.data(), f_FileSize);

            f_FileStream.close();

            ////////////////////////////////////////////// Success! //////////////////////////////////////////////

            return true;
        }
    }//namespace FileIO

    struct Serializer
    {
    public:
        Serializer() = delete;
        ~Serializer() = default;

    public:

        template<typename T, typename = void>
        struct is_serializable_struct : false_type {};

        template<typename T>
        struct is_serializable_struct<T, void_t<typename T::peach_serializable_tag>> : std::bool_constant<std::is_default_constructible_v<T>> {};

        template<typename T>
        [[nodiscard]] static bool
            FromJSON
            (
                T& fp_DesiredObject,
                const string& fp_FilePath,
                Logger* logger
            )
        {
            vector<char> f_CharBuffer;
            vector<Token> f_TokenizedJson;

            JSONValue f_TempJSON;

            if (not FileIO::ReadFileIntoCharBuffer(fp_FilePath, { ".json" }, f_CharBuffer, logger)) //get JSON into a string
            {
                logger->Error("Failed to Read JSON", "FromJSON");
                return false;
            }

            f_TokenizedJson.reserve(static_cast<size_t>(f_CharBuffer.size() / 4)); //heurisitic to avoid dynamic resizing overhead

            if (not Tokenize(VectorStream<char>(std::move(f_CharBuffer)), f_TokenizedJson,  logger)) //convert JSON string into a vector of tokens
            {
                logger->Error("Failed to Lex JSON", "FromJSON");
                return false;
            }
            if (not ParseJSON(VectorStream<Token>(std::move(f_TokenizedJson)), f_TempJSON, logger)) //parse the tokens into a valid JSONValue object
            {
                logger->Error("Failed to Parse JSON", "FromJSON");
                return false;
            }
            if (not FromJSON(f_TempJSON, fp_DesiredObject, logger)) //retrieve values and insert into fp_DesiredObject
            {
                logger->Error(fmt::format("Failed to retrieve data values from desired JSON file: {}", fp_FilePath), "FromJSON");
                return false;
            }

            return true;
        }

        template<typename T>
        [[nodiscard]] static bool
            ToJSON
            (
                const T& fp_DesiredObject,
                const string& fp_DesiredFileName,
                const string& fp_DesiredOutputDirectory,
                Logger* logger
            )
        {         
            JSONValue f_TempJSON = ToJSON(fp_DesiredObject);

            string f_JsonString;
                
            ToString(&f_JsonString, f_TempJSON);

            if (not FileIO::WriteStringToFile(fp_DesiredOutputDirectory, fp_DesiredFileName + ".json", f_JsonString, logger))
            {
                logger->Error(fmt::format("Failed writing to JSON file: {}, nothing was done", fp_DesiredFileName), "ToJSON");
                return false;
            }

            return true;
        }

        template<typename T>
        static bool
            PackIntoBinaryVector
            (
                const T& fp_DesiredObject,
                vector<uint8_t>& fp_BinaryVector
            )
        {
            return ToBinary(fp_DesiredObject, fp_BinaryVector); //i'm a fucking genius >O<
        }

        template<typename T>
        static bool
            UnpackFromBinaryVector
            (
                T& fp_EmptyObject,
                const vector<uint8_t>& fp_BinaryVector,
                Logger* logger,
                size_t& fp_StartReadOffset //start at beginning of vector by default owo
            )
        {
            return FromBinary(fp_EmptyObject, fp_BinaryVector, fp_StartReadOffset, logger);
        }

        template<typename T>
        static bool
            UnpackFromBinaryVector
            (
                T& fp_EmptyObject,
                const vector<uint8_t>& fp_BinaryVector,
                Logger* logger
            )
        {
            size_t f_StartReadOffset = 0;
            return FromBinary(fp_EmptyObject, fp_BinaryVector, f_StartReadOffset, logger);
        }

    private:
        //////////////////////////////////////////////
        // Helper Struct for Parsing
        //////////////////////////////////////////////

        template<typename T>
        struct VectorStream
        {
            explicit 
                VectorStream(vector<T>&& fp_Vector) : pm_Vector(move(fp_Vector)) {}

            [[nodiscard]] bool 
                IsEmpty() 
                const noexcept
            { 
                return pm_Position >= pm_Vector.size();
            }

            [[nodiscard]] bool
                ShiftForward(T& fp_Out)
            {
                if (IsEmpty())
                {
                    return false;
                }

                fp_Out =  pm_Vector[pm_Position++];

                return true;
            }

            void
                ShiftForwardUnsafe(T& fp_Out)
            {
                fp_Out = pm_Vector[pm_Position++];
            }

            [[nodiscard]] bool
                Peek(T& fp_Out)
                const
            {
                if (IsEmpty())
                {
                    return false;
                }

                fp_Out = pm_Vector[pm_Position];

                return true;
            }

            [[nodiscard]] bool
                Peek(size_t fp_Index, T& fp_Out)
                const
            {
                if (fp_Index == 0)   // invalid by definition, if 0 size_t underflows after decrement
                {
                    return false;
                }

                --fp_Index; //need decrement since the current index is the next element since ShiftForward only allows catching the current index

                if (pm_Position + fp_Index >= pm_Vector.size())
                {
                    return false;
                }

                fp_Out = pm_Vector[pm_Position + fp_Index];

                return true;
            }

        private:
            const vector<T> pm_Vector;
            size_t pm_Position = 0;
        };

        //////////////////////////////////////////////
        // Token and Token-type Definition for JSON
        //////////////////////////////////////////////

        enum class TokenType
        {
            //////////////////// GOATS ////////////////////

            IntLiteral,
            FloatLiteral,
            StringLiteral,
            NullLiteral,
            BoolLiteral,

            //////////////////// Bracket Types ////////////////////

            OpenBracket,
            CloseBracket,

            OpenSquareBracket,
            CloseSquareBracket,

            //////////////////// Symbols ////////////////////

            DoubleDot, // ':'
            Comma,

            //////////////////// End Of File ////////////////////

            ENDF,

            //////////////////// Default Value ////////////////////

            NO_TOKEN_VALUE
        };

        struct Token
        {
            string m_Value;
            TokenType m_Type;
            size_t m_SourceCodeLineNumber;

            explicit 
                Token(const string& fp_Value, const TokenType fp_Type, const int fp_SourceCodeLineNumber) : m_Value(fp_Value), m_Type(fp_Type), m_SourceCodeLineNumber(fp_SourceCodeLineNumber) {}

            explicit 
                Token(const char fp_Value, const TokenType fp_Type, const int fp_SourceCodeLineNumber) : m_Value(1, fp_Value), m_Type(fp_Type), m_SourceCodeLineNumber(fp_SourceCodeLineNumber) {}

            explicit 
                Token() : m_Value(""), m_Type(TokenType::NO_TOKEN_VALUE), m_SourceCodeLineNumber(0) {}
        };

        //////////////////////////////////////////////
        // Tokenize Function
        //////////////////////////////////////////////

        static bool
            Tokenize
            (
                VectorStream<char>&& fp_SourceCode,
                vector<Token>& fp_Tokens,
                Logger* logger
            )
        {
            size_t f_CurrentLineNumber = 1;
            char f_CurrentChar = 0;

            while (1)
            {
                //////////////////// Iterate Current Character ////////////////////

                if (not fp_SourceCode.ShiftForward(f_CurrentChar)) //idk only way i get nullptr is if vector iterator is at the end
                {
                    break;
                }
                
                //////////////////// Handle Spaces, New-Lines, and Comments ////////////////////

                if (f_CurrentChar == '\n') //used to keep track of what line number we're at in the source code, we only have single line comments, so this is sufficient
                {
                    f_CurrentLineNumber++;
                    continue; //we can shift forwards confidently since we're currently on the newline character
                }
                else if (isspace(f_CurrentChar))
                {
                    continue;
                }

                //////////////////// Handle Digits or Alphabetic Characters ////////////////////

                if (isdigit(f_CurrentChar) or f_CurrentChar == '-') //used for finding floats and ints defined inside the JSON
                {
                    string f_Number(1, f_CurrentChar); // >w<, C++ is weird mang

                    if (not fp_SourceCode.Peek(f_CurrentChar)) //Peek to see if a number is after, if so, shiftforward will just give the same number
                    {
                        logger->Error(fmt::format("Found END__OF__FILE after a random '-' brother! Did you forget to finish your number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                        return false;
                    }

                    while (isdigit(f_CurrentChar))
                    {
                        fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was used before UwU

                        f_Number += f_CurrentChar;

                        if (not fp_SourceCode.Peek(f_CurrentChar))
                        {
                            logger->Error(fmt::format("Expected a number but found END__OF__FILE! Did you forget to finish your number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                            return false;
                        }
                    }

                    bool f_IsDecimal = false;
                    bool f_IsExp = false;

                    //f_CurrentChar = '.', or a comma or smth
                    if (f_CurrentChar == '.') //used for handling decimal numbers eg. "let x->float = 3.14;"
                    {
                        f_IsDecimal = true;

                        f_Number += f_CurrentChar; //add the decimal so we're at: "69. (rest to be parsed)" currently
                        fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was called inside the while loop before breaking

                        if (not fp_SourceCode.Peek(f_CurrentChar)) //object wasn't properly closed, unterminated UwU!
                        {
                            logger->Error(fmt::format("Expected a number but found END__OF__FILE! Did you forget to finish your number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                            return false;
                        }

                        if (not isdigit(f_CurrentChar))
                        {
                            logger->Error(fmt::format("Unexpected symbol following a '.' brother!, looks like you've input a non-numeric symbol: '{}' while defining a decimal number at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer");
                            return false;
                        }

                        while (isdigit(f_CurrentChar))
                        {
                            fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was used before UwU

                            f_Number += f_CurrentChar;

                            if (not fp_SourceCode.Peek(f_CurrentChar))
                            {
                                logger->Error(fmt::format("Expected a number but found END__OF__FILE! Did you forget to finish your number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                                return false;
                            }
                        }
                    }

                    //f_CurrentChar is currently on a peeked token so look for scientific notation uwu
                    if (f_CurrentChar == 'e' or f_CurrentChar == 'E')
                    {
                        f_IsExp = true;

                        f_Number += f_CurrentChar;
                        fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was called inside the previous while-loop uwu

                        if (not fp_SourceCode.Peek(f_CurrentChar))
                        {
                            logger->Error(fmt::format("Expected a number but found END__OF__FILE! Did you forget to finish your scientific notation number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                            return false;
                        }

                        if (f_CurrentChar != '+' and f_CurrentChar != '-')
                        {
                            logger->Error(fmt::format("Expected '+' or '-' following scientific-notation but found: '{}' instead! Did you forget to finish your number? Error occured at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer");
                            return false;
                        }

                        f_Number += f_CurrentChar;
                        fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was just called OwO!

                        if (not fp_SourceCode.Peek(f_CurrentChar))
                        {
                            logger->Error(fmt::format("Expected a number but found END__OF__FILE! Did you forget to finish your scientific notation number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                            return false;
                        }

                        if (not isdigit(f_CurrentChar))
                        {
                            logger->Error(fmt::format("Unexpected symbol following a 'e' brother!, looks like you've input a non-numeric symbol: '{}' while defining a scientific notation number at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer");
                            return false;
                        }

                        while (isdigit(f_CurrentChar))
                        {
                            fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was used before UwU

                            f_Number += f_CurrentChar;

                            if (not fp_SourceCode.Peek(f_CurrentChar))
                            {
                                logger->Error(fmt::format("Expected a number but found END__OF__FILE! Did you forget to finish your number? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                                return false;
                            }
                        }
                    }

                    if (f_IsDecimal or f_IsExp) //push a float
                    {
                        fp_Tokens.emplace_back(f_Number, TokenType::FloatLiteral, f_CurrentLineNumber); //No need for a continue here since the current character isnt a digit
                    }
                    else //push an int
                    {	
                        fp_Tokens.emplace_back(f_Number, TokenType::IntLiteral, f_CurrentLineNumber); //No need for a continue here since the current character isnt a digit
                    }

                    continue; //move to next iteration
                }
                else if (isalpha(f_CurrentChar)) //used for finding bools and null literals inside the JSON
                {
                    string f_Identifier(1, f_CurrentChar); //start with checked char

                    if (not fp_SourceCode.Peek(f_CurrentChar))
                    {
                        logger->Error(fmt::format("Expected a alphabetic character but found END__OF__FILE! Did you misspell false, true or null? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                        return false;
                    }

                    while (isalpha(f_CurrentChar))
                    {
                        fp_SourceCode.ShiftForwardUnsafe(f_CurrentChar); //don't needa check again since Peek() was used before UwU

                        f_Identifier += f_CurrentChar;

                        if (not fp_SourceCode.Peek(f_CurrentChar))
                        {
                            logger->Error(fmt::format("Expected a alphabetic character but found END__OF__FILE! Did you misspell false, true or null? Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                            return false;
                        }
                    }

                    if (f_Identifier == "true" or f_Identifier == "false")
                    {
                        fp_Tokens.emplace_back(f_Identifier, TokenType::BoolLiteral, f_CurrentLineNumber);
                    }
                    else if (f_Identifier == "null")
                    {
                        fp_Tokens.emplace_back(f_Identifier, TokenType::NullLiteral, f_CurrentLineNumber);
                    }
                    else
                    {
                        logger->Error(fmt::format("Lexing Error: Invalid JSON identifier: '{}', found at line number: {}", f_Identifier, f_CurrentLineNumber), "Lexer");
                        return false;
                    }

                    continue; //move to next iteration since
                }

                switch (f_CurrentChar)
                {
                case ':':
                    fp_Tokens.emplace_back(f_CurrentChar, TokenType::DoubleDot, f_CurrentLineNumber);
                    break;
                case ',':
                    fp_Tokens.emplace_back(f_CurrentChar, TokenType::Comma, f_CurrentLineNumber);
                    break;

                case '{':
                    fp_Tokens.emplace_back(f_CurrentChar, TokenType::OpenBracket, f_CurrentLineNumber);
                    break;
                case '}':
                    fp_Tokens.emplace_back(f_CurrentChar, TokenType::CloseBracket, f_CurrentLineNumber);
                    break;

                case '[':
                    fp_Tokens.emplace_back(f_CurrentChar, TokenType::OpenSquareBracket, f_CurrentLineNumber);
                    break;
                case ']':
                    fp_Tokens.emplace_back(f_CurrentChar, TokenType::CloseSquareBracket, f_CurrentLineNumber);
                    break;
                case '"': //VERY IMPORTANT THAT WE PROCESS THIS BEFORE '/' otherwise '/' mentioned inside of strings might be ignored
                {
                    string f_CurrentStringLiteral = "";

                    // Shift to the next character to start capturing the string, not the opening quote

                    if (not fp_SourceCode.ShiftForward(f_CurrentChar)) //invalid string, unterminated UwU!
                    {
                        logger->Error("Unterminated string literal, brother! Error occured at line number: " + to_string(f_CurrentLineNumber), "Lexer");
                        return false;
                    }

                    while (f_CurrentChar != '"')
                    {
                        if (f_CurrentChar == '\\' and fp_SourceCode.ShiftForward(f_CurrentChar))
                        {
                            switch (f_CurrentChar)
                            {
                                case 'n':
                                    f_CurrentStringLiteral += '\n'; // Add a newline character
                                    break;
                                case 't':
                                    f_CurrentStringLiteral += '\t'; // Add a tab character
                                    break;
                                case '\\':
                                    f_CurrentStringLiteral += '\\'; // Add a literal backslash
                                    break;
                                case '"':
                                    f_CurrentStringLiteral += '"'; // Add a literal double quote
                                    break;
                                default:
                                    // Handle unknown escape sequences or add a fallback behavior
                                    f_CurrentStringLiteral += '\\'; // Re-add the backslash as it was part of the input
                                    f_CurrentStringLiteral += f_CurrentChar; // Add the unknown character as is
                                    break;
                            }
                        }
                        else
                        {
                            f_CurrentStringLiteral += f_CurrentChar;
                        }

                        if (not fp_SourceCode.ShiftForward(f_CurrentChar)) //invalid string, unterminated UwU!
                        {
                            logger->Error(fmt::format("Unterminated string literal, brother! Error occured at line number: {}", f_CurrentLineNumber), "Lexer");
                            return false;
                        }
                    }

                    // Check if we've ended on the closing quotation mark
                    if (f_CurrentChar != '"')
                    {
                        logger->Error(fmt::format("Unterminated string literal, brother! Error occured at line number: {}", f_CurrentLineNumber), "Lexer"); // Handle error: Unterminated string literal, and exit program execution
                        return false;
                    }

                    // Push the final string token without the quotes
                    fp_Tokens.emplace_back(f_CurrentStringLiteral, TokenType::StringLiteral, f_CurrentLineNumber); //lexer will shift on next iteration, f_CurrentChar is pointing -> ' " ' 
                }
                break;
                default:
                    logger->Error(fmt::format("Lexing Error: Unrecognized character found: [{}], found at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer");
                    return false;
                }
            }

            fp_Tokens.emplace_back("", TokenType::ENDF, f_CurrentLineNumber); //label the end of the file i guess for some reason

            return true; //fuck C++
        }

        //////////////////////////////////////////////
        // JSON Parsing
        //////////////////////////////////////////////
        struct JSONValue;

        using JSONObject = unordered_map<string, JSONValue>; //used for regular JSONObjects
        using JSONArray = vector<JSONValue>; //used for JSON arrays and vectors

        struct JSONValue
        {
            enum class Type
            {
                Object,
                Array,
                String,
                Integer,
                UnsignedInteger,
                Float,
                Boolean,
                Null
            } JSONType;

            variant //idk im lazy and sick of using templates
            <
                JSONObject, 
                JSONArray, 
                string, 
                bool,

                int64_t, 
                uint64_t,
                double 
            > m_Value;

            JSONValue() : JSONType(Type::Null), m_Value(false) {}

            explicit JSONValue(JSONObject __obj) : JSONType(Type::Object), m_Value(std::move(__obj)) {}
            explicit JSONValue(JSONArray __arr) : JSONType(Type::Array), m_Value(std::move(__arr)) {}

            explicit JSONValue(string __str) : JSONType(Type::String), m_Value(std::move(__str)) {}
            explicit JSONValue(bool __b) : JSONType(Type::Boolean), m_Value(__b) {}

            template<typename I, enable_if_t<is_integral_v<I>&& is_signed_v<I>, int> = 0>
            JSONValue(I __i) : JSONType(Type::Integer), m_Value(static_cast<int64_t>(__i)) {}

            template<typename I, enable_if_t<is_integral_v<I>&& is_unsigned_v<I>, int> = 0>
            JSONValue(I __i) : JSONType(Type::UnsignedInteger), m_Value(static_cast<uint64_t>(__i)) {}

            JSONValue(double __f) : JSONType(Type::Float), m_Value(__f) {} //not explicit to implicitly cast float -> double
        };

        //////////////////////////////////////////////
        // JSON Utility Functions
        //////////////////////////////////////////////

        static bool
            ToString
            (
                string* fp_JSONString, 
                const JSONValue& fp_JSON
            ) //kicks off recursive creation of JSON string
        {
            if (not fp_JSONString)
            {
                PRINT_ERROR("Passed nullptr reference to string, ToString() is not possible exiting function call immediately");
                return false;
            }

            stringstream f_TempString;

            if (not ToStringStream(fp_JSON, f_TempString))
            {
                PRINT_ERROR("Unable to stringify JSON");
                return false;
            }

            *fp_JSONString = f_TempString.str();
            return true;
        }

        static void
            EscapeJSONString
            (
                const string& fp_In, 
                stringstream& fp_Out
            )
        {
            fp_Out << '"';

            for (unsigned char lv_CurrentChar : fp_In)
            {
                switch (lv_CurrentChar)
                {
                case '\"':
                    fp_Out << "\\\"";
                    break;
                case '\\':
                    fp_Out << "\\\\";
                    break;
                case '\b':
                    fp_Out << "\\b";
                    break;
                case '\f':
                    fp_Out << "\\f";
                    break;
                case '\n':
                    fp_Out << "\\n";
                    break;
                case '\r':
                    fp_Out << "\\r";
                    break;
                case '\t':
                    fp_Out << "\\t";
                    break;
                default:
                    if (lv_CurrentChar < 0x20)
                    {
                        // escape other control chars as \u00XX
                        static const char* hex = "0123456789ABCDEF";
                        fp_Out << "\\u00"
                            << hex[(lv_CurrentChar >> 4) & 0xF]
                            << hex[lv_CurrentChar & 0xF];
                    }
                    else
                    {
                        // UTF-8 bytes (including 🍑) just pass through
                        fp_Out << lv_CurrentChar;
                    }
                    break;
                }
            }

            fp_Out << '"';
        }

        static bool
            ToStringStream
            (
                const JSONValue& fp_JSONValue,
                stringstream& fp_JSONString,
                const uint32_t fp_Spacing = 0
            )
        {
            string f_IndentLevel(fp_Spacing, ' ');

            switch (fp_JSONValue.JSONType)
            {
                case JSONValue::Type::String:
                {
                    const string& sv_String = get<string>(fp_JSONValue.m_Value);
                    EscapeJSONString(sv_String, fp_JSONString);
                }
                break;
                case JSONValue::Type::Null:
                    fp_JSONString << "null";
                    break;
                case JSONValue::Type::Boolean:
                    fp_JSONString << (get<bool>(fp_JSONValue.m_Value) ? "true" : "false");
                    break;
                case JSONValue::Type::Integer:
                    fp_JSONString << get<int64_t>(fp_JSONValue.m_Value);
                    break;
                case JSONValue::Type::Float:
                    fp_JSONString << setprecision(numeric_limits<double>::max_digits10) << defaultfloat << get<double>(fp_JSONValue.m_Value); //have to do this to preserve precision using iostreams 
                    break;
                case JSONValue::Type::UnsignedInteger:
                    fp_JSONString << get<uint64_t>(fp_JSONValue.m_Value);
                    break;
                case JSONValue::Type::Array:
                {
                    const auto& arr = get<JSONArray>(fp_JSONValue.m_Value);
                    string f_ScopeIndent = f_IndentLevel + string(4, ' '); //add a 4 space indent for the scope

                    fp_JSONString << "[" << "\n" << f_ScopeIndent; //start array, advance to next line, and indent for scope

                    size_t f_Indexer = 0; //used for tracking if the col width is rlly long because i want pretty jsons uwu

                    for (auto _it = arr.begin(); _it != arr.end(); ++_it)
                    {
                        ToStringStream(*_it, fp_JSONString, fp_Spacing + 4); //4 spaces for indent level

                        if (next(_it) == arr.end()) //do this before adding any new lines to avoid double new lines for prettyness >w<
                        {
                            break; //break so we dont add an extra comma after the end has been reached
                        }

                        fp_JSONString << ", "; //add comma until we hit the last element

                        if (_it->JSONType == JSONValue::Type::Object or _it->JSONType == JSONValue::Type::Array) //XXX: we're assuming mono typed arrays so no mixing of objects/arrays and primitive types
                        {
                            fp_JSONString << "\n" << f_ScopeIndent; //new line for each JSONObject inside the array
                        }
                        else if (f_Indexer % MAX_ARRAY_LINE_WIDTH == MAX_ARRAY_LINE_WIDTH - 1) //this is an else if so that objects/arrays won't double line
                        {
                            fp_JSONString << "\n" << f_ScopeIndent; //newline every 20 elements for non objects
                        }

                        f_Indexer++;
                    }

                    fp_JSONString << "\n" << f_IndentLevel << "]";
                }
                break;
                case JSONValue::Type::Object:
                {
                    fp_JSONString << "{" << "\n";
                    const auto& obj = get<JSONObject>(fp_JSONValue.m_Value);

                    string f_ScopeIndent = f_IndentLevel + string(4, ' '); //add a 4 space indent for the scope

                    for (auto it = obj.begin(); it != obj.end(); ++it)
                    {
                        fp_JSONString << f_ScopeIndent << '"' << it->first << '"' << ": ";

                        ToStringStream(it->second, fp_JSONString, fp_Spacing + 4); //add 4 for indent level

                        if (next(it) != obj.end()) //check for the end of the container
                        {
                            fp_JSONString << ", "; // comma after each element except the last
                        }

                        fp_JSONString << "\n";
                    }

                    fp_JSONString << f_IndentLevel << "}";
                }
                break;
            }

            return true;
        }

        static void
            PrintToConsole(JSONValue& fp_JSON)
        {
            string f_StringJSON;
            ToString(&f_StringJSON, fp_JSON);
            PRINT(f_StringJSON, Colours::BrightWhite);
        }

        //////////////////////////////////////////////
        // Helper Templates
        //////////////////////////////////////////////
        /*
        These templates are used for detecting data structs that have SERIALIZABLE_FIELDS implemented since serialization in this library completely relies on the defined functions
        provided by the macro to serialize data.

        As well as checking for map/vector structs since serializing them is pretty clean in JSON and honestly are used widely enough that not being able to serialize maps/structs feels
        like a major downside.
        */

        template<typename T, typename = void>
        struct has_reserve : false_type{};

        template<typename T>
        struct has_reserve <T, void_t<decltype(declval<T&>().reserve(declval<typename T::size_type()>))>> : true_type {};

        template<typename T>
        static inline constexpr bool has_reserve_v = has_reserve<T>::value;

        template<typename T, typename = void>
        struct is_queue : false_type {};

        template<typename T>
        struct is_queue<T, 
            void_t<
                typename T::value_type,
                decltype(declval<T>().pop())
            >
        > : true_type {};

        template<typename T, typename = void>
        struct is_map : false_type {};

        template<typename T>
        struct is_map<T, void_t< //XXX: used for deducing general map types
            typename T::key_type,
            typename T::mapped_type,
            decltype(declval<T>().begin()),
            decltype(declval<T>().end())
            >> : true_type {};

        template<typename T>
        struct is_basic_string : false_type {};

        template<typename CharT, typename Traits, typename Alloc>
        struct is_basic_string<std::basic_string<CharT, Traits, Alloc>> : true_type {}; //the std:: here makes it easier for me to read between is_ and basic_string uwu

        template<typename T, typename = void>
        struct is_vector : false_type {};

        template<typename T>
        struct is_vector<T, void_t<
            typename T::value_type,
            decltype(declval<T>().begin()),
            decltype(declval<T>().end())
            >> : bool_constant<not is_basic_string<T>::value> {};

        template <class T>
        struct is_unique_ptr : std::false_type {}; //stds here make it easier to read uwu!

        template <class T, class D>
        struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type {};

        template <class T>
        static inline constexpr bool is_unique_ptr_v = is_unique_ptr<std::remove_cvref_t<T>>::value;

        template <class T>
        struct unique_ptr_pointee;

        template <class T, class D>
        struct unique_ptr_pointee<std::unique_ptr<T, D>> { using type = T; };

        template <class T>
        using unique_ptr_pointee_t = typename unique_ptr_pointee<std::remove_cvref_t<T>>::type;

        using BINARY_STRING_LENGTH_V = uint64_t; //to make things standard 

        //////////////////////////////////////////////
        // Main (De)Serialization Functions
        //////////////////////////////////////////////
        /*
        These functions should be used on POD structs, serializing more complex data types is fine, however serialization for basic types is only supported since I dont really see any reason
        for serializing more complex types, since it just amounts to serializing everything down to integral types since that's what computers at their foundation understand + string because
        strings are super common so we deal with those uwu.

        For any external types, just create a POD struct that encapsulates fields relevant to reconstructing the external type and assign them manually.

        This is the case since JSON expects and does things just fine using basic types + strings + lists/POD structs.
        */

        //==================================================================================================================================================================//
        
        //////////////////////////////////////////////
        // JSON Serialization
        //////////////////////////////////////////////

        template<typename T>        
        static JSONValue
            ToJSON(const T& fp_ObjectToSerialize) //IT WORKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKSSS IM SO TIRED >w< ;w; i sleep like a champion tn
        {
            JSONObject f_Object; // ✅ this is what i was missin UwU

            auto f_Visitor = [&f_Object](const char* fp_Name, auto&& fp_Value)
            {
                using FieldType = decay_t<decltype(fp_Value)>; //makes things look prettier

                if constexpr (is_arithmetic_v<FieldType> or is_basic_string<FieldType>::value)
                {
                    //JSONValue constructor call will auto assign appropriate type since we utilize explicit constructors
                    f_Object.emplace(fp_Name, fp_Value);
                }
                else if constexpr (is_serializable_struct<FieldType>::value)
                {
                    f_Object.emplace(fp_Name, Serializer::ToJSON(fp_Value));
                }
                else if constexpr (is_map<FieldType>::value)
                {
                    JSONObject f_MapObj;

                    using MapValType = typename FieldType::mapped_type;

                    for (const auto& [lv_MapKey, lv_MapVal] : fp_Value)
                    {
                        if constexpr (is_serializable_struct<MapValType>::value)
                        {
                            f_MapObj.emplace(lv_MapKey, Serializer::ToJSON(lv_MapVal));
                        }
                        //XXX: this is used for vector keys
                        else if constexpr (is_vector<MapValType>::value) //is constexpr here kosher idk, future me: yeah it is
                        {
                            JSONArray f_TempArray;
                            Serializer::ToJSONArray(f_TempArray, lv_MapVal);
                            f_MapObj.emplace(lv_MapKey, f_TempArray);
                        }
                        else
                        {
                            f_MapObj.emplace(lv_MapKey, lv_MapVal);
                        }
                    }

                    f_Object.emplace(fp_Name, f_MapObj);
                }
                else if constexpr (is_vector<FieldType>::value)
                {
                    JSONArray arr;
                    Serializer::ToJSONArray(arr, fp_Value);
                    f_Object.emplace(fp_Name, arr);
                }
                else
                {
                    static_assert(always_false_v<FieldType>, "Unsupported field type in ToJSON, did you forget a macro definition?");
                }
            };

            fp_ObjectToSerialize.PEACH_VISIT(f_Visitor);

            return JSONValue(std::move(f_Object));
        }

        /*
        used for parsing vectors -> JSONArrays and deals with nested vectors since ToJSON requires SERIALIZABLE_FIELDS structs by default
        */
        template<typename T_VectorObject>
        static void
            ToJSONArray
            (
                JSONArray& fp_ArrayObject, 
                const T_VectorObject& fp_SerializableObjectField
            ) 
        {
            using VectorElem = typename decay_t<decltype(fp_SerializableObjectField)>::value_type;

            for (const auto& lv_VectorVal : fp_SerializableObjectField)
            {
                if constexpr (is_serializable_struct<VectorElem>::value)
                {
                    fp_ArrayObject.emplace_back(ToJSON(lv_VectorVal)); //this handles custom structs 
                }
                else if constexpr (is_basic_string<VectorElem>::value or is_arithmetic_v<VectorElem>)
                {
                    fp_ArrayObject.emplace_back(lv_VectorVal);
                }
                else if constexpr (is_vector<VectorElem>::value)
                {
                    JSONArray f_NestedArray;
                    ToJSONArray(f_NestedArray, lv_VectorVal); //this handles nested vectors
                    fp_ArrayObject.emplace_back(f_NestedArray);
                }
                else
                {
                    static_assert(always_false_v<decltype(lv_VectorVal)>, "Unsupported field type in ToJSONArray");
                }
            }
        }

        //==================================================================================================================================================================//

        template<typename T> 
        [[nodiscard]] static bool //leverages SERIALIZE_FIELD function defs to assign values to a default constructed data struct
            FromJSON
            (
                const JSONValue& fp_JSON, 
                T& fp_OutObject, 
                Logger* logger
            )
        {
            if (fp_JSON.JSONType != JSONValue::Type::Object)
            {
                logger->Error("Passed invalid JSON type to FromJSON()", "FromJSON");
                return false;
            }

            const JSONObject& f_JSONObject = get<JSONObject>(fp_JSON.m_Value);
            bool f_IsSuccessful = true; //XXX: used to track state of lambda execution

            auto f_Visitor = [&f_JSONObject, &f_IsSuccessful, logger](const char* fp_Name, auto& fp_Value)
            {
                if (not f_IsSuccessful) //immediately return since something failed along the way >///< >w<!
                {
                    return;
                }

                auto f_It = f_JSONObject.find(fp_Name);

                if (f_It == f_JSONObject.end()) //SOMETHING BAD HAPPENED WTF, someone hand editied a json or i fucked the dog on this one
                {
                    logger->Error(fmt::format("Missing JSON field '{}', WHAT DID YOU DO, WHAT DID I DO WTF???", fp_Name), "FromJSON");
                    f_IsSuccessful = false;
                    return;
                }

                using FieldType = decay_t<decltype(fp_Value)>;

                try
                {
                    const JSONValue& f_JsonValue = f_It->second;

                    if constexpr (is_basic_string<FieldType>::value or is_arithmetic_v<FieldType>)
                    {
                        fp_Value = Serializer::Extract<FieldType>(f_JsonValue);
                    }
                    else if constexpr (is_serializable_struct<FieldType>::value)
                    {
                        if (not Serializer::FromJSON(f_JsonValue, fp_Value, logger))
                        {
                            logger->Error("failed to deserialize non primitive struct inside JSON Object", "FromJSON");
                            f_IsSuccessful = false;
                            return; //exit early UwU!
                        }
                    }
                    else if constexpr (is_map<FieldType>::value)
                    {
                        const JSONObject& f_MapObject = get<JSONObject>(f_JsonValue.m_Value);
                        fp_Value.clear(); //clear the map in case the user passes a map filled with values

                        using ValType = typename FieldType::mapped_type; //can do this since FieldType is guaranteed a map uwu, holy shit nostradamus is AHHHHH record of ragnarock mang

                        for (const auto& [lv_MapKey, lv_Val] : f_MapObject)
                        {
                            ValType f_Item{};

                            if constexpr (is_serializable_struct<ValType>::value)
                            {
                                if (not Serializer::FromJSON(lv_Val, f_Item, logger))
                                {
                                    logger->Error("failed to deserialize non primitive struct inside JSON Object", "FromJSON");
                                    f_IsSuccessful = false;
                                    return; //exit early uwu
                                }
                            }
                            //XXX: this is used for nested vectors
                            else if constexpr (is_vector<ValType>::value) //is constexpr here kosher idk, future me: yeah it is
                            {
                                const JSONArray& arr = get<JSONArray>(lv_Val.m_Value);

                                if (not Serializer::FromJSONArray(arr, f_Item, logger)) //OwO!
                                {
                                    f_IsSuccessful = false;
                                    return;
                                }
                            }
                            else
                            {
                                f_Item = Serializer::Extract<ValType>(lv_Val);
                            }

                            fp_Value.emplace(lv_MapKey, move(f_Item));
                        }
                    }
                    else if constexpr (is_vector<FieldType>::value) //XXX: don't need to check for string types here since we already do so at the first branch
                    {
                        const JSONArray& arr = get<JSONArray>(f_JsonValue.m_Value);

                        if (not Serializer::FromJSONArray(arr, fp_Value, logger))
                        {
                            logger->Error(fmt::format("Failed to deserialize vector for field '{}'", fp_Name), "FromJSON");
                            f_IsSuccessful = false;
                            return;
                        }
                    }
                }
                catch (const exception& fp_Exception)
                {
                    logger->Error(fmt::format("Deserialization failed for field '{}' (type: {}): {}", fp_Name, typeid(decltype(fp_Value)).name(), fp_Exception.what()), "FromJSON");
                    f_IsSuccessful = false;
                }
            };

            fp_OutObject.PEACH_VISIT(f_Visitor);

            return f_IsSuccessful;
        }

        template<typename T>
        static T 
            Extract(const JSONValue& fp_JSON) //we extract and recast anything like doubles and 64 bit ints -> whatever the user defined eg vector<int>
        {
            using FieldType = decay_t<T>;

            if constexpr (is_basic_string<FieldType>::value) 
            {
                return get<string>(fp_JSON.m_Value);
            }
            else if constexpr (is_same_v<FieldType, bool>)
            {
                return get<bool>(fp_JSON.m_Value);
            }
            else if constexpr (is_arithmetic_v<FieldType>) //AHHH IT FUCKING WORKS, have to wrap in this if otherwise the compiler bitches
            {
                switch (fp_JSON.JSONType) //XXX: this switch statement is absolutely necessary since the parser doesn't know what the original type requires for its variable width
                {
                case JSONValue::Type::Integer:
                    return static_cast<FieldType>(get<int64_t>(fp_JSON.m_Value)); //all numbers with a leading '-' are coerced into a int64_t

                case JSONValue::Type::UnsignedInteger:
                    return static_cast<FieldType>(get<uint64_t>(fp_JSON.m_Value)); //all positive integers are coerced into a uint64_t

                case JSONValue::Type::Float:
                    return static_cast<FieldType>(get<double>(fp_JSON.m_Value)); //any decimal number is coerced into a double

                case JSONValue::Type::Boolean: // allow bool -> numeric
                    return static_cast<FieldType>(get<bool>(fp_JSON.m_Value));

                default:
                    throw runtime_error("Extract<T>: JSON type is not numeric but is aritmetic UwU!");
                }
            }
            else 
            {
                // Static error w/ full type sig
                #if defined(_MSC_VER)
                    static_assert(always_false_v<T>, "Unsupported type in Extract. Check __FUNCSIG__ for details: " __FUNCSIG__);
                #else
                    static_assert(always_false_v<T>, "Unsupported type in Extract. Check __func__ for details: " );
                    //static_assert(always_false_v<T>, "Unsupported type in Extract. Check __PRETTY_FUNCTION__ for details: " __PRETTY_FUNCTION__);
                #endif
            }
        }

        template<typename T_VectorObject>
        static bool
            FromJSONArray
            (
                const JSONArray& fp_ArrayObject, 
                T_VectorObject& fp_SerializableObjectField,
                Logger* logger
            )
        {
            fp_SerializableObjectField.clear(); //clear the vector in case the user passes a vector filled with values

            using Elem = typename decay_t<decltype(fp_SerializableObjectField)>::value_type;

            for (const auto& lv_VectorVal : fp_ArrayObject)
            {
                Elem item{};

                try
                {
                    if constexpr (is_serializable_struct<Elem>::value)
                    {
                        if (not FromJSON(lv_VectorVal, item, logger))
                        {
                            logger->Error("failed to deserialize non primitive struct", "FromJSONArray");
                            return false;
                        }
                    }
                    else if constexpr (is_vector<Elem>::value) //XXX: used for nested vectors, needa check for strings since they're just char vectors
                    {
                        const JSONArray& f_NestedArray = get<JSONArray>(lv_VectorVal.m_Value); //peel back vector one layer at a time
                        //call again assuming Elem reduces to a vector type and at the lowest level it will fill item with primitives or serializable structs
                        if(not FromJSONArray(f_NestedArray, item, logger))
                        {
                            logger->Error("Failed to deserialize nested vector element" , "FromJSONArray");
                            return false;
                        }
                    }
                    else
                    {
                        item = Extract<Elem>(lv_VectorVal);
                    }
                }
                catch (const exception& e)
                {
                    logger->Error(fmt::format("Deserialization failed in FromJSONArray (type: '{}'): {}", typeid(Elem).name(), e.what()), "FromJSONArray");
                    return false;
                }

                fp_SerializableObjectField.push_back(item);
            }

            return true; //success! JSONArray was deserialized >W<
        }

        //==================================================================================================================================================================//

        //////////////////////////////////////////////
        // Binary Serialization
        //////////////////////////////////////////////

        //In general, the To functions don't need explicit error handling because it's all done at compile time, however reading can throw because ppl are dumb or files can corrupt

        template<typename T>
        [[nodiscard]] static bool
            ToBinary
            (
                const T& fp_ObjectToSerialize,
                vector<uint8_t>& fp_BinaryWriteVector
            ) 
        {
            auto f_Visitor = [&fp_BinaryWriteVector](const char* fp_Name, auto&& fp_Value)
            {
                using FieldType = decay_t<decltype(fp_Value)>; //makes things look prettier

                if constexpr (is_serializable_struct<FieldType>::value)
                {
                    Serializer::ToBinary(fp_Value, fp_BinaryWriteVector);
                }
                else if constexpr (is_map<FieldType>::value) //WARNING: oof never use unordered_map here UNLESS u know what ur doing owo
                {
                    Serializer::MapToBinary(fp_Value, fp_BinaryWriteVector);
                }
                else if constexpr (is_vector<FieldType>::value)
                {
                    Serializer::VectorToBinary(fp_Value, fp_BinaryWriteVector);
                }
                else
                {
                    Serializer::ValueToBinary(fp_Value, fp_BinaryWriteVector);
                }
            };

            fp_ObjectToSerialize.PEACH_VISIT(f_Visitor);

            return true;
        }

        template<typename T_VectorObject>
        static void
            VectorToBinary
            (
                const T_VectorObject& fp_SerializableObjectField,
                vector<uint8_t>& fp_BinaryWriteVector
            )
        {
            static_assert(is_vector<T_VectorObject>::value, "[INTERNAL ERROR]: attempted to pass non vector object into VectorToBinary()");

            BinaryCodec::EncodeInt<uint64_t>(fp_BinaryWriteVector,static_cast<uint64_t>(fp_SerializableObjectField.size())); //since it's a vector we push the size first uwu

            using VectorElem = typename decay_t<decltype(fp_SerializableObjectField)>::value_type;

            for (const auto& lv_VectorVal : fp_SerializableObjectField)
            {
                if constexpr (is_serializable_struct<VectorElem>::value)
                {
                    ToBinary(lv_VectorVal, fp_BinaryWriteVector); //this handles custom structs 
                }
                else if constexpr (is_vector<VectorElem>::value)
                {
                    VectorToBinary(lv_VectorVal, fp_BinaryWriteVector);
                }
                else if constexpr (is_map<VectorElem>::value)
                {
                    MapToBinary(lv_VectorVal, fp_BinaryWriteVector);
                }
                else
                {
                    ValueToBinary(lv_VectorVal, fp_BinaryWriteVector);
                }
            }
        }

        template<typename T_MapObject>
        static void
            MapToBinary
            (
                const T_MapObject& fp_SerializableObjectField,
                vector<uint8_t>& fp_BinaryWriteVector
            )
        {
            static_assert(is_map<T_MapObject>::value, "[INTERNAL ERROR]: attempted to pass non map object into MapToBinary()");

            BinaryCodec::EncodeInt<uint64_t>(fp_BinaryWriteVector, static_cast<uint64_t>(fp_SerializableObjectField.size())); //since it's a map we push the size first uwu

            using MapValType = typename T_MapObject::mapped_type;
            using MapKeyType = typename T_MapObject::key_type;

            for (const auto& [lv_MapKey, lv_MapVal] : fp_SerializableObjectField)
            {
                ////////////////////////////////////////////// Map Keys //////////////////////////////////////////////

                if constexpr (is_serializable_struct<MapKeyType>::value)
                {
                    ToBinary(lv_MapKey, fp_BinaryWriteVector);
                }
                //XXX: this is used for vector keys
                else if constexpr (is_vector<MapKeyType>::value) //is constexpr here kosher idk, future me: yeah it is
                {
                    VectorToBinary(lv_MapKey, fp_BinaryWriteVector);
                }
                else if constexpr (is_map<MapKeyType>::value)
                {
                    MapToBinary(lv_MapKey, fp_BinaryWriteVector);
                }
                else
                {
                    ValueToBinary(lv_MapKey, fp_BinaryWriteVector);
                }

                ////////////////////////////////////////////// Map Values //////////////////////////////////////////////

                if constexpr (is_serializable_struct<MapValType>::value)
                {
                    ToBinary(lv_MapVal, fp_BinaryWriteVector);
                }
                //XXX: this is used for vector keys
                else if constexpr (is_vector<MapValType>::value) //is constexpr here kosher idk, future me: yeah it is
                {
                    VectorToBinary(lv_MapVal, fp_BinaryWriteVector);
                }
                else if constexpr (is_map<MapValType>::value)
                {
                    MapToBinary(lv_MapVal, fp_BinaryWriteVector);
                }
                else
                {
                    ValueToBinary(lv_MapVal, fp_BinaryWriteVector);
                }
            }
        }

        template<typename T>
        static void
            ValueToBinary
            (
                const T& fp_Value,
                vector<uint8_t>& fp_BinaryWriteVector
            )
        {
            using ValType = remove_cvref_t<T>;

            if constexpr (is_same_v<ValType, float>)
            {
                BinaryCodec::EncodeFloat(fp_BinaryWriteVector, fp_Value);
            }
            else if constexpr (is_same_v<ValType, double>)
            {
                BinaryCodec::EncodeDouble(fp_BinaryWriteVector, fp_Value);
            }
            else if constexpr (is_same_v<ValType, bool>)
            {
                BinaryCodec::EncodeBool(fp_BinaryWriteVector, fp_Value);
            }
            else if constexpr (is_arithmetic_v<ValType>)
            {
                BinaryCodec::EncodeInt<ValType>(fp_BinaryWriteVector, fp_Value);
            }
            else if constexpr (is_basic_string<ValType>::value)
            {
                BinaryCodec::EncodeStringUTF8<BINARY_STRING_LENGTH_V>(fp_BinaryWriteVector, fp_Value);
            }
            else if constexpr (is_unique_ptr_v<ValType>)
            {
                bool f_IsNotNull = static_cast<bool>(fp_Value); //convert ptr address -> bool
                BinaryCodec::EncodeBool(fp_BinaryWriteVector, f_IsNotNull);

                if (f_IsNotNull) //encode only if not null >w<
                {
                    using Pointee = unique_ptr_pointee_t<ValType>;

                    if constexpr (is_serializable_struct<Pointee>::value)
                    {
                        ToBinary(*fp_Value, fp_BinaryWriteVector);
                    }
                    else if constexpr (is_vector<Pointee>::value)
                    {
                        VectorToBinary(*fp_Value, fp_BinaryWriteVector);
                    }
                    else if constexpr (is_map<Pointee>::value)
                    {
                        MapToBinary(*fp_Value, fp_BinaryWriteVector);
                    }
                    else
                    {
                        ValueToBinary(*fp_Value, fp_BinaryWriteVector);
                    }
                }
            }
            else if constexpr (is_pointer_v<ValType>)
            {
                static_assert(always_false_v<T>, "Only unique_ptr is supported for serialization, shared_ptr, weak_ptr and raw pointers are not supported!");
            }
            else
            {
                static_assert(always_false_v<decltype(fp_Value)>, "Unsupported value type in ValueToBinary()");
            }
        }

        template<typename T>
        [[nodiscard]] static bool //leverages SERIALIZE_FIELD function defs to assign values to a default constructed data struct
            FromBinary
            (
                T& fp_OutObject,
                const vector<uint8_t>& fp_BinaryReadVector,
                size_t& fp_CurrentOffset,
                Logger* logger
            )
        {
            if (fp_CurrentOffset >= fp_BinaryReadVector.size())
            {
                logger->Error("Offset exceeded binary size oooop uwu *pats head", "FromBinary()");
                return false;
            }

            bool f_IsSuccessful = true; //XXX: used to track state of lambda execution

            auto f_Visitor = [&fp_BinaryReadVector, &fp_CurrentOffset, &f_IsSuccessful, logger](const char* fp_Name, auto& fp_Value)
            {
                if (not f_IsSuccessful) //immediately return since something failed along the way >///< >w<!
                {
                    return;
                }

                using FieldType = decay_t<decltype(fp_Value)>;

                try
                {
                    if constexpr (is_serializable_struct<FieldType>::value)
                    {
                        if (not Serializer::FromBinary(fp_Value, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error(fmt::format("Failed to deserialize non primitive struct from binary for field named: '{}'", fp_Name), "FromBinary");
                            f_IsSuccessful = false;
                            return; //exit early UwU!
                        }
                    }
                    else if constexpr (is_map<FieldType>::value)
                    {
                        if (not Serializer::MapFromBinary(fp_Value, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error(fmt::format("Failed to deserialize map for field named: '{}'", fp_Name), "FromBinary");
                            f_IsSuccessful = false;
                            return;
                        }
                    }
                    else if constexpr (is_vector<FieldType>::value) //XXX: don't need to check for string types here since we already do so at the first branch
                    {
                        if (not Serializer::VectorFromBinary(fp_Value, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error(fmt::format("Failed to deserialize vector for field named: '{}'", fp_Name), "FromBinary");
                            f_IsSuccessful = false;
                            return;
                        }
                    }
                    else
                    {
                        if (not Serializer::ValueFromBinary(fp_Value, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error(fmt::format("Failed to deserialize value for field named: '{}'", fp_Name), "FromBinary");
                            f_IsSuccessful = false; //tell outer function thatis false uwu
                            return; //return from lambda
                        }
                    }
                    
                }
                catch (const exception& fp_Exception)
                {
                    logger->Error(fmt::format("Deserialization failed for field '{}' (type: {}): {}", fp_Name, typeid(decltype(fp_Value)).name(), fp_Exception.what()), "FromBinary");
                    f_IsSuccessful = false;
                    return;
                }
            };

            fp_OutObject.PEACH_VISIT(f_Visitor);

            return f_IsSuccessful;
        }

        template<typename T>
        static bool
            ValueFromBinary
            (
                T& fp_OutValue,
                const vector<uint8_t>& fp_BinaryReadVector,
                size_t& fp_CurrentOffset,
                Logger* logger
            ) 
        {
            using ValType = remove_cvref_t<T>;

            if constexpr (is_same_v<ValType, float>)
            {
                fp_OutValue = BinaryCodec::DecodeFloat(fp_BinaryReadVector, fp_CurrentOffset);
            }
            else if constexpr (is_same_v<ValType, double>)
            {
                fp_OutValue = BinaryCodec::DecodeDouble(fp_BinaryReadVector, fp_CurrentOffset);
            }
            else if constexpr (is_same_v<ValType, bool>)
            {
                fp_OutValue = BinaryCodec::DecodeBool(fp_BinaryReadVector, fp_CurrentOffset);
            }
            else if constexpr (is_arithmetic_v<ValType>)
            {
                fp_OutValue = BinaryCodec::DecodeInt<ValType>(fp_BinaryReadVector, fp_CurrentOffset);
            }
            else if constexpr (is_basic_string<ValType>::value)
            {
                fp_OutValue = BinaryCodec::DecodeStringUTF8<BINARY_STRING_LENGTH_V>(fp_BinaryReadVector, fp_CurrentOffset);
            }
            else if constexpr (is_unique_ptr_v<ValType>)
            {
                bool f_HasValue = BinaryCodec::DecodeBool(fp_BinaryReadVector, fp_CurrentOffset);

                if (f_HasValue) //decode only if not null >w<
                {
                    using Pointee = unique_ptr_pointee_t<ValType>;

                    fp_OutValue = make_unique<Pointee>(); //REQUIRES DEFAULT CONSTRUCTION, integral types works, basic_string/vec/map works, and serializable_struct works since we check uwu

                    if constexpr (is_serializable_struct<Pointee>::value)
                    {
                        FromBinary(*fp_OutValue, fp_BinaryReadVector);
                    }
                    else if constexpr (is_vector<Pointee>::value)
                    {
                        VectorFromBinary(*fp_OutValue, fp_BinaryReadVector, fp_CurrentOffset, logger);
                    }
                    else if constexpr (is_map<Pointee>::value)
                    {
                        MapFromBinary(*fp_OutValue, fp_BinaryReadVector, fp_CurrentOffset, logger);
                    }
                    else
                    {
                        ValueFromBinary(*fp_OutValue, fp_BinaryReadVector, fp_CurrentOffset, logger);
                    }
                }
            }
            else if constexpr (is_pointer_v<ValType>)
            {
                static_assert(always_false_v<T>, "Only unique_ptr is supported for serialization, shared_ptr, weak_ptr and raw pointers are not supported!");
            }
            else
            {
                // Static error w/ full type sig
#if defined(_MSC_VER)
                static_assert(always_false_v<T>, "Unsupported type in Extract. Check __FUNCSIG__ for details: " __FUNCSIG__);
#else
                static_assert(always_false_v<T>, "Unsupported type in Extract, Did you manually edit or pass a binary not generated by Serializer.h?");
#endif
            }

            return true;
        }

        template<typename T_VectorObject>
        static bool
            VectorFromBinary
            (
                T_VectorObject& fp_OutVector,
                const vector<uint8_t>& fp_BinaryReadVector,
                size_t& fp_CurrentOffset,
                Logger* logger
            )
        {
            static_assert(is_vector<T_VectorObject>::value, "[INTERNAL ERROR]: attempted to pass non vector object into VectorFromBinary()");

            size_t f_AmountOfVectorElements = static_cast<size_t>(BinaryCodec::DecodeInt<uint64_t>(fp_BinaryReadVector, fp_CurrentOffset));

            fp_OutVector.clear(); //clear the vector in case the user passes a vector filled with values

            fp_OutVector.reserve(f_AmountOfVectorElements); //allocate memory to avoid realloc overhead

            using Elem = typename decay_t<decltype(fp_OutVector)>::value_type;

            for (size_t lv_CurrentIndex = 0; lv_CurrentIndex < f_AmountOfVectorElements; lv_CurrentIndex++) //uwu
            {
                fp_OutVector.emplace_back(); // constructs Elem in place
                Elem& fv_CurrentItem = fp_OutVector.back(); //grab a reference and fill it uwu!

                try
                {
                    if constexpr (is_serializable_struct<Elem>::value)
                    {
                        if (not FromBinary(fv_CurrentItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error("failed to deserialize non primitive struct", "VectorFromBinary");
                            return false;
                        }
                    }
                    else if constexpr (is_vector<Elem>::value) //XXX: used for nested vectors, needa check for strings since they're just char vectors
                    {
                        //call again assuming Elem reduces to a vector type and at the lowest level it will fill item with primitives or serializable structs
                        if (not VectorFromBinary(fv_CurrentItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error("Failed to deserialize nested vector element", "VectorFromBinary");
                            return false;
                        }
                    }
                    else if constexpr (is_map<Elem>::value)
                    {
                        if (not MapFromBinary(fv_CurrentItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                        {
                            logger->Error("Failed to deserialize nested map element", "VectorFromBinary");
                            return false;
                        }
                    }
                    else if(not ValueFromBinary(fv_CurrentItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                    {
                        logger->Error("Failed to deserialize value element", "VectorFromBinary");
                        return false;
                    }
                }
                catch (const exception& fp_Exception)
                {
                    logger->Error(fmt::format("Deserialization failed in VectorFromBinary() (type: '{}'): {}", typeid(Elem).name(), fp_Exception.what()), "VectorFromBinary");
                    return false;
                }
            }

            return true; //success! Vector was deserialized >W<
        }

        template<typename T_MapObject>
        static bool
            MapFromBinary
            (
                T_MapObject& fp_OutMap,
                const vector<uint8_t>& fp_BinaryReadVector,
                size_t& fp_CurrentOffset,
                Logger* logger
            )
        {
            static_assert(is_map<T_MapObject>::value, "[INTERNAL ERROR]: attempted to pass non map object into MapToBinary()");

            using MapValType = typename T_MapObject::mapped_type; //can do this since T_MapObject is guaranteed a map uwu, holy shit nostradamus is AHHHHH record of ragnarock mang
            using MapKeyType = typename T_MapObject::key_type;

            size_t f_AmountOfMapElements = static_cast<size_t>(BinaryCodec::DecodeInt<uint64_t>(fp_BinaryReadVector, fp_CurrentOffset));

            fp_OutMap.clear(); //clear the map in case the user passes a map filled with values

            if constexpr (has_reserve_v<T_MapObject>) //reserve when applicable uwu
            {
                fp_OutMap.reserve(f_AmountOfMapElements);
            }

            for (size_t lv_CurrentIndex = 0; lv_CurrentIndex < f_AmountOfMapElements; lv_CurrentIndex++) //OwO        
            {
                ////////////////////////////////////////////// Map Keys //////////////////////////////////////////////

                MapKeyType fv_CurrentMapKeyItem{};

                if constexpr (is_serializable_struct<MapKeyType>::value)
                {
                    if (not FromBinary(fv_CurrentMapKeyItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                    {
                        logger->Error("failed to deserialize map key from binary for a given serializable struct", "MapFromBinary");
                        return false; //exit early uwu
                    }
                }
                //XXX: this is used for nested vectors
                else if constexpr (is_vector<MapKeyType>::value) //is constexpr here kosher idk, future me: yeah it is
                {
                    if (not VectorFromBinary(fv_CurrentMapKeyItem, fp_BinaryReadVector, fp_CurrentOffset, logger)) //OwO!
                    {
                        logger->Error("failed to deserialize nested map key from binary for a given vector", "MapFromBinary");
                        return false;
                    }
                }
                else if constexpr (is_map<MapKeyType>::value)
                {
                    if (not MapFromBinary(fv_CurrentMapKeyItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                    {
                        logger->Error("failed to deserialize nested map key from binary for a given map", "MapFromBinary");
                        return false;
                    }
                }
                else 
                {
                    if (not ValueFromBinary(fv_CurrentMapKeyItem, fp_BinaryReadVector, fp_CurrentOffset, logger))
                    {
                        logger->Error("Failed to deserialize map key element!", "MapFromBinary");
                        return false;
                    }
                }

                ////////////////////////////////////////////// Map Values //////////////////////////////////////////////

                auto [fv_KeyIt, fv_IsInserted] = fp_OutMap.try_emplace(move(fv_CurrentMapKeyItem));

                if (not fv_IsInserted) //THIS WILL NOT WORK FOR DUPLICATED KEYS UWU
                {
                    logger->Error("Duplicate key in binary map", "MapFromBinary");
                    return false;
                }

                if constexpr (is_serializable_struct<MapValType>::value)
                {
                    if (not FromBinary(fv_KeyIt->second, fp_BinaryReadVector, fp_CurrentOffset, logger))
                    {
                        logger->Error("failed to deserialize map value from binary for a given serializable struct", "MapFromBinary");
                        return false; //exit early uwu
                    }
                }
                //XXX: this is used for nested vectors
                else if constexpr (is_vector<MapValType>::value) //is constexpr here kosher idk, future me: yeah it is
                {
                    if (not VectorFromBinary(fv_KeyIt->second, fp_BinaryReadVector, fp_CurrentOffset, logger)) //OwO!
                    {
                        logger->Error("failed to deserialize vector value from binary for a given vector", "MapFromBinary");
                        return false;
                    }
                }
                else if constexpr (is_map<MapValType>::value)
                {
                    if (not MapFromBinary(fv_KeyIt->second, fp_BinaryReadVector, fp_CurrentOffset, logger))
                    {
                        logger->Error("failed to deserialize nested map value from binary for a given map", "MapFromBinary");
                        return false;
                    }
                }
                else
                {
                    if((not ValueFromBinary(fv_KeyIt->second, fp_BinaryReadVector, fp_CurrentOffset, logger)))
                    {
                        logger->Error("Failed to deserialize map value element!", "MapFromBinary");
                        return false;
                    }
                }
            }

            return true;
        }

    private:
        //////////////////////////////////////////////
        // Parsing Functions
        //////////////////////////////////////////////

        static bool
            ParseObject //assuming that this is only called when '{' is found uwu
            (
                VectorStream<Token>& fp_Tokens,
                JSONObject& fp_JSONObject, //current list containing the entire parsed JSON up to this point
                Logger* logger
            )
        {
            string f_CurrentKey;
            Token f_CurrentToken;

            fp_Tokens.ShiftForward(f_CurrentToken); //look for ':'

            while (f_CurrentToken.m_Type != TokenType::CloseBracket) //this will break out of the loop if it parses towards ENDF for invalid JSONS in the worst cases
            {
                if (f_CurrentToken.m_Type != TokenType::StringLiteral)
                {
                    logger->Error(fmt::format("Parsing Error: found '{}', when string literal was expected as JSON key inside object at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject");
                    return false;
                }

                f_CurrentKey = std::move(f_CurrentToken.m_Value);
                fp_Tokens.ShiftForward(f_CurrentToken); //look for ':'

                if (f_CurrentToken.m_Type != TokenType::DoubleDot)
                {
                    logger->Error(fmt::format("Parsing Error: found '{}', when ':' was expected after JSON key inside object at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject");
                    return false;
                }

                fp_Tokens.ShiftForward(f_CurrentToken); //look for value associated with key

                if (not ParseValue(fp_Tokens, f_CurrentToken, fp_JSONObject, f_CurrentKey, logger))
                {
                    logger->Error(fmt::format("Parsing Error: Invalid JSON object: '{}', at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject");
                    return false;
                }

                fp_Tokens.ShiftForward(f_CurrentToken); //look for comma or close bracket

                if (f_CurrentToken.m_Type == TokenType::CloseBracket)
                {
                    break;
                }

                if (f_CurrentToken.m_Type != TokenType::Comma)
                {
                    logger->Error(fmt::format("Parsing Error: found '{}', when ',' was expected after JSON value inside object at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject");
                    return false;
                }

                fp_Tokens.ShiftForward(f_CurrentToken); // consume comma, and look for next key value pair
            }

            if (f_CurrentToken.m_Type != TokenType::CloseBracket)
            {
                logger->Error(fmt::format("Parsing Error: Unexpected token: [{}], found inside array definition at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject");
                return false;
            }

            return true;
        }

        static bool
            ParseArray //assuming the most recent token was '[' called from ParseJSON
            (
                VectorStream<Token>& fp_Tokens,
                JSONArray& fp_JSONArray, //current list containing the entire parsed JSON up to this point
                Logger* logger
            )
        {
            Token f_CurrentToken;

            fp_Tokens.ShiftForward(f_CurrentToken); //look for ':'

            while (f_CurrentToken.m_Type != TokenType::CloseSquareBracket) //this will break out of the loop if it parses towards ENDF for invalid JSONS in the worst cases
            {
                if (not ParseValue(fp_Tokens, f_CurrentToken, fp_JSONArray, logger))
                {
                    logger->Error("Parsing Error: invalid value found while parsing an Array", "ParseArray");
                    return false;
                }

                fp_Tokens.ShiftForward(f_CurrentToken); //shift to find comma

                if (f_CurrentToken.m_Type == TokenType::CloseSquareBracket) // check for end of array before we check for comma
                {
                    break;
                }

                if (f_CurrentToken.m_Type != TokenType::Comma) //throw error if a separating comma is not found between array elements
                {
                    logger->Error(fmt::format("Parsing Error: expected ',' after value inside JSON array but found '{}' instead at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseArray");
                    return false;
                }

                fp_Tokens.ShiftForward(f_CurrentToken); //shift past the comma to find the next value
            }

            if (f_CurrentToken.m_Type != TokenType::CloseSquareBracket)
            {
                logger->Error(fmt::format("Parsing Error: Expected ']' but found '{}' instead, found inside array definition at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseArray");
                return false;
            }

            return true;
        }

        static bool
            ParseValue //used for parsing values inside an array
            (
                VectorStream<Token>& fp_Tokens,
                const Token& fp_CurrentToken,
                JSONArray& fp_Array,
                Logger* logger
            )
        {
            switch (fp_CurrentToken.m_Type)
            {
                case TokenType::StringLiteral:
                    fp_Array.emplace_back(fp_CurrentToken.m_Value);
                    break;
                case TokenType::IntLiteral:
                    if (fp_CurrentToken.m_Value[0] == '-') //store any positive number as a uint64 because y not we'll recast it at deserialization
                    {
                        fp_Array.emplace_back(static_cast<int64_t>(stoll(fp_CurrentToken.m_Value))); // signed
                    }
                    else
                    {
                        fp_Array.emplace_back(static_cast<uint64_t>(stoull(fp_CurrentToken.m_Value))); // unsigned
                    }
                    break;
                case TokenType::FloatLiteral:
                    fp_Array.emplace_back(stod(fp_CurrentToken.m_Value));
                    break;
                case TokenType::BoolLiteral:
                    fp_Array.emplace_back(fp_CurrentToken.m_Value == "true");
                    break;
                case TokenType::NullLiteral:
                    fp_Array.emplace_back(); //lmfao this looks so dumb but works
                    break;

                case TokenType::OpenBracket: //check for nested objects
                {
                    JSONObject f_TempObject;
                    ParseObject(fp_Tokens, f_TempObject, logger);
                    fp_Array.emplace_back(f_TempObject);
                }
                break;
                case TokenType::OpenSquareBracket: //check for nested arrays
                {
                    JSONArray f_TempArray;
                    ParseArray(fp_Tokens, f_TempArray, logger);
                    fp_Array.emplace_back(f_TempArray);
                }
                break;
                default:
                    logger->Error(fmt::format("Parsing Error: found '{}' inside array, when integral type was expected at line number: {}", fp_CurrentToken.m_Value, fp_CurrentToken.m_SourceCodeLineNumber), "ParseValue");
                    return false;
            }

            return true;
        }

        static bool
            ParseValue //used for parsing values inside a regular JSON object
            (
                VectorStream<Token>& fp_Tokens,
                const Token& fp_CurrentToken,
                JSONObject& fp_JSONObject,
                const string& fp_ValueKey,
                Logger* logger
            )
        {
            switch (fp_CurrentToken.m_Type)
            {
                case TokenType::StringLiteral:
                    fp_JSONObject.emplace(fp_ValueKey, fp_CurrentToken.m_Value);
                    break;
                case TokenType::IntLiteral:
                    if (fp_CurrentToken.m_Value[0] == '-') //XXX: this is used to handle container sizing issues coming from values serialized as a large uint64 vs a regular int64
                    {
                        fp_JSONObject.emplace(fp_ValueKey, static_cast<int64_t>(stoll(fp_CurrentToken.m_Value))); // signed
                    }
                    else
                    {
                        fp_JSONObject.emplace(fp_ValueKey, static_cast<uint64_t>(stoull(fp_CurrentToken.m_Value))); // unsigned
                    }
                    break;
                case TokenType::FloatLiteral:
                    fp_JSONObject.emplace(fp_ValueKey, stod(fp_CurrentToken.m_Value));
                    break;
                case TokenType::BoolLiteral:
                    fp_JSONObject.emplace(fp_ValueKey, fp_CurrentToken.m_Value == "true");
                    break;
                case TokenType::NullLiteral:
                    fp_JSONObject.emplace(fp_ValueKey, JSONValue());
                    break;

                case TokenType::OpenBracket:
                {
                    JSONObject f_TempObject;
                    ParseObject(fp_Tokens, f_TempObject, logger);
                    fp_JSONObject.emplace(fp_ValueKey, f_TempObject);
                }
                break;
                case TokenType::OpenSquareBracket:
                {
                    JSONArray f_TempArray;
                    ParseArray(fp_Tokens, f_TempArray, logger);
                    fp_JSONObject.emplace(fp_ValueKey, f_TempArray);
                }
                break;
                default:
                    logger->Error(fmt::format("Parsing Error: found '{}' inside object, when integral type was expected at line number: {}", fp_CurrentToken.m_Value, fp_CurrentToken.m_SourceCodeLineNumber), "ParseValue");
                    return false;
            }

            return true;
        }

        static bool //XXX: this function assumes that the JSON is structured such that it has one top level object denoted by a "{ . . . . }"
            ParseJSON //function call that kicks off the recursive parse chain
            (
                VectorStream<Token>&& fp_Tokens,
                JSONValue& fp_JSON,
                Logger* logger
            )
        {
            Token f_CurrentToken;

            if (not fp_Tokens.ShiftForward(f_CurrentToken)) //get first val
            {
                logger->Error("Empty Token array was fed into ParseJSON()", "Lexer");
                return false;
            }

            switch (f_CurrentToken.m_Type) //should only need to do this once for a valid JSON
            {
                case TokenType::OpenBracket:
                {
                    JSONObject f_Object;
                    ParseObject(fp_Tokens, f_Object, logger);
                    fp_JSON = std::move(JSONValue(f_Object));
                }
                break;
                case TokenType::OpenSquareBracket:
                {
                    JSONArray f_Array;
                    ParseArray(fp_Tokens, f_Array, logger);
                    fp_JSON = std::move(JSONValue(f_Array));
                }
                break;
                default:
                    logger->Error("Parsing Error: ill-formed JSON found, parsing failed", "ParseJSON");
                    return false;
            }

            return true;
        }
    };
}



//uwu 1069
