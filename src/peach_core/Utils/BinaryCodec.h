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
#include <vector>
#include <type_traits>
#include <cstring> //for memcpy uwu
#include <string>
#include <stdexcept>
#include <limits>

/// C std 
#include <stdint.h>


#if defined(_MSC_VER)
    #define PEACH_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define PEACH_FORCEINLINE inline __attribute__((always_inline))
#else
    // Fallback for anything else
    #define PEACH_FORCEINLINE inline
#endif

// Add these to your platform defines header
#if defined(_MSC_VER)
    #include <stdlib.h>
    #define PEACH_BSWAP16(x) _byteswap_ushort(x)
    #define PEACH_BSWAP32(x) _byteswap_ulong(x)
    #define PEACH_BSWAP64(x) _byteswap_uint64(x)
#elif defined(__GNUC__) || defined(__clang__)
    #define PEACH_BSWAP16(x) __builtin_bswap16(x)
    #define PEACH_BSWAP32(x) __builtin_bswap32(x)
    #define PEACH_BSWAP64(x) __builtin_bswap64(x)
#else
    // Portable fallback — constant-folds on any decent optimizer.
    #define PEACH_BSWAP16(x) \
        (uint16_t)(((uint16_t)(x) >> 8) | ((uint16_t)(x) << 8))
    #define PEACH_BSWAP32(x) ( \
        (((uint32_t)(x) & 0xFF000000u) >> 24) | \
        (((uint32_t)(x) & 0x00FF0000u) >>  8) | \
        (((uint32_t)(x) & 0x0000FF00u) <<  8) | \
        (((uint32_t)(x) & 0x000000FFu) << 24))
    #define PEACH_BSWAP64(x) ( \
        (((uint64_t)(x) & 0xFF00000000000000ull) >> 56) | \
        (((uint64_t)(x) & 0x00FF000000000000ull) >> 40) | \
        (((uint64_t)(x) & 0x0000FF0000000000ull) >> 24) | \
        (((uint64_t)(x) & 0x000000FF00000000ull) >>  8) | \
        (((uint64_t)(x) & 0x00000000FF000000ull) <<  8) | \
        (((uint64_t)(x) & 0x0000000000FF0000ull) << 24) | \
        (((uint64_t)(x) & 0x000000000000FF00ull) << 40) | \
        (((uint64_t)(x) & 0x00000000000000FFull) << 56))
#endif

namespace PeachCore{

    template<typename>
    inline constexpr bool always_false_v = false;

}

namespace PeachCore::BinaryCodec::Utility{
    
    template<typename T>
    [[nodiscard]] PEACH_FORCEINLINE T
        BSwap
        (
            const T fp_Val
        )
    {
        static_assert(sizeof(T) == 1 or sizeof(T) == 2 or sizeof(T) == 4 or sizeof(T) == 8, "BSwap: unsupported width — only 1/2/4/8-byte types are supported");
 
        // 1-byte types (char, bool, int8_t) don't need swapping
        if constexpr (sizeof(T) == 1) 
        {            
            return fp_Val;
        }

        using BitType = 
            std::conditional_t
            <
                sizeof(T) == 2, uint16_t, 
                std::conditional_t
                <
                    sizeof(T) == 4, uint32_t, 
                    uint64_t
                >
            >
        ;

        BitType f_Bits;
        std::memcpy(&f_Bits, &fp_Val, sizeof(T));

        if constexpr (sizeof(T) == 2) 
        {            
            f_Bits = PEACH_BSWAP16(f_Bits);
        }        
        else if constexpr (sizeof(T) == 4) 
        {            
            f_Bits = PEACH_BSWAP32(f_Bits);
        }        
        else 
        {        
            f_Bits = PEACH_BSWAP64(f_Bits);
        }

        T f_Result;
        std::memcpy(&f_Result, &f_Bits, sizeof(T));
        return f_Result;
    }

    template <bool tp_IsBigEndian, typename T>
    PEACH_FORCEINLINE void 
        EncodeNumber
        (
            std::vector<uint8_t>& fp_Bytes, 
            const T fp_ArithmeticVal
        )
    {
        static_assert(std::is_arithmetic_v<T>, "EncodeNumber() only accepts arithmetic types (int, uint, float, double, bool)");

        if constexpr (std::is_same_v<T, bool>) //no standard size for bool so we just clamp it to 1 byte
        {
            uint8_t f_BoolAsInt = fp_ArithmeticVal ? 1 : 0; // Convert boolean to 8-bit integer
            fp_Bytes.push_back(f_BoolAsInt);
        }
        else if constexpr(tp_IsBigEndian)
        {
            const size_t f_Old = fp_Bytes.size();
            fp_Bytes.resize(f_Old + sizeof(T));
            memcpy(fp_Bytes.data() + f_Old, &BSwap(fp_ArithmeticVal), sizeof(T));
        }
        else 
        {
            const size_t f_Old = fp_Bytes.size();
            fp_Bytes.resize(f_Old + sizeof(T));
            memcpy(fp_Bytes.data() + f_Old, &fp_ArithmeticVal, sizeof(T));
        }
    }

    template <bool tp_IsBigEndian, typename T>
    [[nodiscard]] PEACH_FORCEINLINE T
        DecodeNumber
        (
            const std::vector<uint8_t>& fp_Bytes, 
            size_t& fp_Offset
        )
    {
        static_assert(std::is_arithmetic_v<T>, "DecodeNumber only accepts arithmetic types, you tried to pass a non integer type");

        if constexpr(std::is_same_v<T, bool>) //bool isnt guaranteed to be 1 byte by the standard so w/e
        {
            if (fp_Offset + 1 > fp_Bytes.size())
            {
                throw std::runtime_error("BinaryCodec: out-of-bounds read");
            }

            return fp_Bytes[fp_Offset++]; //just a numbah owo
        }
        else
        {
            if (fp_Offset + sizeof(T) > fp_Bytes.size()) [[unlikely]]
            {
                throw std::runtime_error("BinaryCodec: out-of-bounds read");
            }
            
            T f_DecodedValue;
            memcpy(&f_DecodedValue, fp_Bytes.data() + fp_Offset, sizeof(T));
            fp_Offset += sizeof(T);

            if constexpr(tp_IsBigEndian)
            {
               return BSwap(f_DecodedValue);
            }

            return f_DecodedValue;
        }
    }

    template<bool tp_IsBigEndian, typename LengthT>
    PEACH_FORCEINLINE void
        EncodeStringUTF8
        (
            std::vector<uint8_t>& fp_Bytes,
            const std::string& fp_String
        )
    {
        ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

        static_assert
        (
            std::is_same_v<LengthT, uint8_t> or std::is_same_v<LengthT, uint16_t> or std::is_same_v<LengthT, uint32_t> or std::is_same_v<LengthT, uint64_t>,
            "EncodeStringUTF8 only accepts 8/16/32/64-bit unsigned integer types"
        );

        ////////////////////////////////////////////// Encode String Length //////////////////////////////////////////////

        size_t f_StringLengthOffset = fp_Bytes.size(); //length index, no -1 needed since we adding the length after we stored this owo

        //add space for slot since we gon write it after
        fp_Bytes.resize(f_StringLengthOffset + sizeof(LengthT), 0); //needa resize her since we want elements here and the string write wont do that

        const size_t f_DataStartOffset = fp_Bytes.size();
        fp_Bytes.reserve(f_DataStartOffset + fp_String.size()); //reserve space to avoid re alloc overhead and to allow simd opt on a fixed size container

        ////////////////////////////////////////////// Write Character by Character //////////////////////////////////////////////

        const char* fv_SegStart = fp_String.data();
        const char* fv_End      = fp_String.data() + fp_String.size();

        while(1)
        {
            const char* fv_Hit = strpbrk(fv_SegStart, "\n\t\\");

            // clamp to end if no escape chars remain in this segment
            if (fv_Hit == nullptr || fv_Hit >= fv_End)
            {
                fv_Hit = fv_End;
            }

            // bulk insert the clean segment before the hit
            if (fv_Hit > fv_SegStart)
            {
                fp_Bytes.insert
                (
                    fp_Bytes.end(),
                    reinterpret_cast<const uint8_t*>(fv_SegStart),
                    reinterpret_cast<const uint8_t*>(fv_Hit)
                );
            }


            if (fv_Hit == fv_End)
            {
                break;
            }

            switch (*fv_Hit)
            {
            case '\n':  // Newline
                fp_Bytes.push_back('\\');
                fp_Bytes.push_back('n');
                break;
            case '\t':  // Tab
                fp_Bytes.push_back('\\');
                fp_Bytes.push_back('t');
                break;
            case '\\':  // Backslash
                fp_Bytes.push_back('\\');
                fp_Bytes.push_back('\\');
                break;
            default:
                break;
            }

            fv_SegStart = fv_Hit + 1;
        }

        // we add sizeof(LengthT) because we are working on a byte array, 
        // so the offset before encoding was N, and after we called EncodeNumber<LengthT> it's N + sizeof(LengthT)
        const size_t f_FinalEncodedStringLength = fp_Bytes.size() - f_DataStartOffset;

        ////////////////////////////////////////////// Overflow Check on String Size With Passed Type //////////////////////////////////////////////

        if (f_FinalEncodedStringLength > std::numeric_limits<LengthT>::max()) [[unlikely]]
        {
            // Roll back to leave fp_ByteCode consistent before throwing
            fp_Bytes.resize(f_StringLengthOffset);
            fp_Bytes.shrink_to_fit(); // hint to release excess capacity, not guaranteed by standard but all major impls honour it
            throw std::length_error("EncodeStringUTF8: string too long for given type");
        }

        ////////////////////////////////////////////// Patch Length After String Resolution uwu //////////////////////////////////////////////


        if constexpr(tp_IsBigEndian)
        {
            LengthT f_FinalLength = BSwap(static_cast<LengthT>(f_FinalEncodedStringLength));
            memcpy(fp_Bytes.data() + f_StringLengthOffset, &f_FinalLength, sizeof(LengthT));
        }
        else 
        {
            LengthT f_FinalLength = static_cast<LengthT>(f_FinalEncodedStringLength);
            memcpy(fp_Bytes.data() + f_StringLengthOffset, &f_FinalLength, sizeof(LengthT));
        }
    }

    template<bool tp_IsBigEndian, typename LengthT>
    PEACH_FORCEINLINE void 
        EncodeStringWithoutEscapeCharacters
        (
            std::vector<uint8_t>& fp_Bytes,
            const std::string& fp_String
        )
    {
        ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

        static_assert
        (
            std::is_same_v<LengthT, uint8_t> or std::is_same_v<LengthT, uint16_t> or std::is_same_v<LengthT, uint32_t> or std::is_same_v<LengthT, uint64_t>,
            "EncodeStringWithoutEscapeCharacters() only accepts 8/16/32/64-bit unsigned integer types"
        );

        ////////////////////////////////////////////// Overflow Check on String Size With Passed Type //////////////////////////////////////////////

        if (fp_String.size() > std::numeric_limits<LengthT>::max()) [[unlikely]]
        {
            throw std::length_error("EncodeStringWithoutEscapeCharacters(): string too long for Length Type passed owo");
        }

        ////////////////////////////////////////////// Reserve String Length Count + Bytes //////////////////////////////////////////////
        
        fp_Bytes.reserve(fp_Bytes.size() + sizeof(LengthT) + fp_String.size()); //handles edge case to avoid double re alloc 

        ////////////////////////////////////////////// Encode String Length //////////////////////////////////////////////

        EncodeNumber<tp_IsBigEndian, LengthT>(fp_Bytes, static_cast<LengthT>(fp_String.size()));

        ////////////////////////////////////////////// Insert String at Back >w< //////////////////////////////////////////////

        fp_Bytes.insert(fp_Bytes.end(), fp_String.begin(), fp_String.end());
    }

    template<bool tp_IsBigEndian, typename LengthT>
    [[nodiscard]] PEACH_FORCEINLINE std::string
        DecodeStringWithoutEscapeCharacters
        (
            const std::vector<uint8_t>& fp_Bytes, 
            size_t& fp_Offset
        )
    {
        ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

        static_assert
        (
            std::is_same_v<LengthT, uint8_t> or std::is_same_v<LengthT, uint16_t> or std::is_same_v<LengthT, uint32_t> or std::is_same_v<LengthT, uint64_t>,
            "DecodeStringWithoutEscapeCharacters only accepts 8/16/32/64-bit unsigned integer types"
        );

        ////////////////////////////////////////////// Get String Length //////////////////////////////////////////////

        LengthT f_StringLength = DecodeNumber<tp_IsBigEndian, LengthT>(fp_Bytes, fp_Offset);

        ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

        // not >= because lv_CurrentOffset -> f_StringLength - 1, so if fp_Offset + f_StringLength == size() it's fine

        if(fp_Offset + f_StringLength > fp_Bytes.size()) [[unlikely]]
        {
            throw std::runtime_error("DecodeStringWithoutEscapeCharacters: out of bounds");
        }

        ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

        fp_Offset += f_StringLength; // End on fresh byte right after the last decoded byte uwu, can do this since we just checked bounds owo

        ////////////////////////////////////////////// Create String and Allocate Space //////////////////////////////////////////////

        return std::string(reinterpret_cast<const char*>(fp_Bytes.data() + fp_Offset - f_StringLength), f_StringLength);
    }

    template<bool tp_IsBigEndian, typename LengthT>
    [[nodiscard]] PEACH_FORCEINLINE std::string //idk this one is kinda big but whatever try to inline it
        DecodeStringUTF8
        (
            const std::vector<uint8_t>& fp_Bytes,
            size_t& fp_Offset
        )
    {
        ////////////////////////////////////////////// Validate Length Type //////////////////////////////////////////////

        static_assert
        (
            std::is_same_v<LengthT, uint8_t> or std::is_same_v<LengthT, uint16_t> or std::is_same_v<LengthT, uint32_t> or std::is_same_v<LengthT, uint64_t>,
            "DecodeStringUTF8 only accepts 8/16/32/64-bit unsigned integer types"
        );

        ////////////////////////////////////////////// Get String Length //////////////////////////////////////////////

        LengthT f_StringLength = DecodeNumber<tp_IsBigEndian, LengthT>(fp_Bytes, fp_Offset);

        ////////////////////////////////////////////// Safety Check Bounds //////////////////////////////////////////////

        if (fp_Offset + f_StringLength > fp_Bytes.size()) // not >= because lv_CurrentOffset -> f_StringLength - 1, so if fp_Offset + f_StringLength == size() it's fine
        {
            throw std::runtime_error("DecodeStringUTF8: out of bounds");
        }

        ////////////////////////////////////////////// Create String and Allocate Space //////////////////////////////////////////////

        std::string f_DecodedString;
        f_DecodedString.reserve(f_StringLength); // Reserve space to optimize append operations

        ////////////////////////////////////////////// Decode String //////////////////////////////////////////////

        const char* fv_SegStart = reinterpret_cast<const char*>(fp_Bytes.data() + fp_Offset);
        const char* fv_End      = fv_SegStart + f_StringLength;

        while (true)
        {
            const char* fv_Hit = reinterpret_cast<const char*>(memchr(fv_SegStart, '\\', fv_End - fv_SegStart));

            if (fv_Hit == nullptr)
            {
                fv_Hit = fv_End;
            }

            // bulk insert clean segment
            f_DecodedString.append(fv_SegStart, fv_Hit);

            if (fv_Hit == fv_End)
            {
                break;
            }

            // fv_Hit points at '\\', peek the next byte
            if (fv_Hit + 1 < fv_End)
            {
                switch (*(fv_Hit + 1))
                {
                case 'n':
                    f_DecodedString.push_back('\n');
                    break;
                case 't':
                    f_DecodedString.push_back('\t');
                    break;
                case '\\':
                    f_DecodedString.push_back('\\');
                    break;
                default:
                    f_DecodedString.push_back('\\'); // unrecognised, preserve the backslash
                    f_DecodedString.push_back(*(fv_Hit + 1));
                    break;
                }

                fv_SegStart = fv_Hit + 2; // skip both the '\\' and the escape char
            }
            else
            {
                // trailing lone backslash at end of encoded region, preserve it
                f_DecodedString.push_back('\\');
                break;
            }
        }

        ////////////////////////////////////////////// Shift Offset onto First Leading Byte //////////////////////////////////////////////

        fp_Offset += f_StringLength; // End on fresh byte right after the last decoded byte uwu

        ////////////////////////////////////////////// Return Decoded String //////////////////////////////////////////////

        return f_DecodedString;
    }
}//namespace BinaryCodec::Utility

 namespace PeachCore::BinaryCodec::LittleEndian {

    //////////////////////////////////////////////
    // Encoding Functions
    //////////////////////////////////////////////

    template <typename T>
    PEACH_FORCEINLINE void 
        EncodeNumber
        (
            std::vector<uint8_t>& fp_Bytes, 
            const T fp_ArithmeticVal
        )
    {
        Utility::EncodeNumber<false, T>(fp_Bytes, fp_ArithmeticVal);
    }

    template <typename T>
    [[nodiscard]] PEACH_FORCEINLINE T
        DecodeNumber
        (
            const std::vector<uint8_t>& fp_Bytes, 
            size_t& fp_Offset
        )
    {
        return Utility::DecodeNumber<false, T>(fp_Bytes, fp_Offset);
    }

    template<typename LengthT>
    PEACH_FORCEINLINE void
        EncodeStringUTF8
        (
            std::vector<uint8_t>& fp_Bytes,
            const std::string& fp_String
        )
    {
        Utility::EncodeStringUTF8<false, LengthT>(fp_Bytes, fp_String);
    }

    template<typename LengthT>
    [[nodiscard]] PEACH_FORCEINLINE std::string //idk this one is kinda big but whatever try to inline it
        DecodeStringUTF8
        (
            const std::vector<uint8_t>& fp_Bytes,
            size_t& fp_Offset
        )
    {
        return Utility::DecodeStringUTF8<false, LengthT>(fp_Bytes, fp_Offset);
    }

    template<typename LengthT>
    PEACH_FORCEINLINE void 
        EncodeStringWithoutEscapeCharacters
        (
            std::vector<uint8_t>& fp_Bytes,
            const std::string& fp_String
        )
    {
        Utility::EncodeStringWithoutEscapeCharacters<false, LengthT>(fp_Bytes, fp_String);
    }

    template<typename LengthT>
    [[nodiscard]] PEACH_FORCEINLINE std::string
        DecodeStringWithoutEscapeCharacters
        (
            const std::vector<uint8_t>& fp_Bytes, 
            size_t& fp_Offset
        )
    {
        return Utility::DecodeStringWithoutEscapeCharacters<false, LengthT>(fp_Bytes, fp_Offset);
    }

}//namespace BinaryCodec::LittleEndian

namespace PeachCore::BinaryCodec::BigEndian{

    template <typename T>
    PEACH_FORCEINLINE void 
        EncodeNumber
        (
            std::vector<uint8_t>& fp_Bytes, 
            const T fp_ArithmeticVal
        )
    {
        Utility::EncodeNumber<true, T>(fp_Bytes, fp_ArithmeticVal);
    }

    template <typename T>
    [[nodiscard]] PEACH_FORCEINLINE T
        DecodeNumber
        (
            const std::vector<uint8_t>& fp_Bytes, 
            size_t& fp_Offset
        )
    {
        return Utility::DecodeNumber<true, T>(fp_Bytes, fp_Offset);
    }

    template<typename LengthT>
    PEACH_FORCEINLINE void
        EncodeStringUTF8
        (
            std::vector<uint8_t>& fp_Bytes,
            const std::string& fp_String
        )
    {
        Utility::EncodeStringUTF8<true, LengthT>(fp_Bytes, fp_String);
    }

    template<typename LengthT>
    [[nodiscard]] PEACH_FORCEINLINE std::string //idk this one is kinda big but whatever try to inline it
        DecodeStringUTF8
        (
            const std::vector<uint8_t>& fp_Bytes,
            size_t& fp_Offset
        )
    {
        return Utility::DecodeStringUTF8<true, LengthT>(fp_Bytes, fp_Offset);
    }

    template<typename LengthT>
    PEACH_FORCEINLINE void 
        EncodeStringWithoutEscapeCharacters
        (
            std::vector<uint8_t>& fp_Bytes,
            const std::string& fp_String
        )
    {
        Utility::EncodeStringWithoutEscapeCharacters<true, LengthT>(fp_Bytes, fp_String);
    }

    template<typename LengthT>
    [[nodiscard]] PEACH_FORCEINLINE std::string
        DecodeStringWithoutEscapeCharacters
        (
            const std::vector<uint8_t>& fp_Bytes, 
            size_t& fp_Offset
        )
    {
        return Utility::DecodeStringWithoutEscapeCharacters<true, LengthT>(fp_Bytes, fp_Offset);
    }

}//namespace BinaryCodec::BigEndian