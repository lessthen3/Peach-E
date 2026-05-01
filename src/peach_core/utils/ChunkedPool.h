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
#include <memory>
#include <cstdint>

namespace PeachCore
{
    using namespace std;

    namespace
    {
        static inline consteval size_t //stole this from moody_camel's queue impl w creds
            CeilToPow2(size_t x)
        {
            // From http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
            --x;

            x |= x >> 1;
            x |= x >> 2;
            x |= x >> 4;

            for (size_t i = 1; i < sizeof(size_t); i <<= 1) 
            {
                x |= x >> (i << 3);
            }

            ++x;

            return x;
        }
    }

    template<typename ValueType, size_t BLOCK_SIZE = 1024>
    class ChunkedPool
    {
        static_assert(BLOCK_SIZE == CeilToPow2(BLOCK_SIZE), "ChunkedPool Block Size must be a power of 2");
        static_assert(BLOCK_SIZE >= 2, "ChunkedPool Block Size must be at least 2");

    public:
        struct Handle
        {
            uint64_t Index = 0;      // global slot index
            uint32_t Generation = 0; // for stale handle detection
        };

    private:
        struct Slot
        {
            ValueType        Value{};
            uint32_t Generation = 0;
            bool     InUse = false;
        };

        struct Block
        {
            Slot Slots[BLOCK_SIZE];
            Block* NextBlock = nullptr;
        };

        vector<unique_ptr<Block>> pm_Blocks; //master recordd
        vector<Handle>            pm_FreeList;
        vector<ValueType> pm_LoadingDock;

        unique_ptr<Block> pm_InitialBlock{nullptr}; //initial block for iterating

        size_t pm_BackIndex = 0; // Number of slots logically addressable so far
        size_t pm_NextAvailableSlotIndex = 0;

    public:
        ChunkedPool() = default;

        // Non-const version (for reading and writing)
        ValueType& 
            operator[](const size_t fp_Index) 
        {
            if(fp_Index < BLOCK_SIZE)
            {
                return pm_InitialBlock->Slots[fp_Index];
            }

            size_t f_BlockNum = fp_Index / BLOCK_SIZE; //will floor when truncated uwu

            Block* f_CurrentBlock = pm_InitialBlock->NextBlock;

            for(f_BlockNum; f_BlockNum > 1; --f_BlockNum)
            {
                if(not (f_CurrentBlock = f_CurrentBlock->NextBlock))
                {
                    throw overflow_error("Tried to access index out of bounds on ChunkedPool!");
                }
            }
            // Add bounds checking for safety if needed
            return f_CurrentBlock->Slots[fp_Index];
        }

        // Const version (for read-only access)
        const ValueType& 
            operator[](const size_t fp_Index) 
            const
        {
            if(fp_Index < BLOCK_SIZE)
            {
                return pm_InitialBlock->Slots[fp_Index];
            }

            size_t f_BlockNum = fp_Index / BLOCK_SIZE; //will floor when truncated uwu

            Block* f_CurrentBlock = pm_InitialBlock->NextBlock;

            for(f_BlockNum; f_BlockNum > 1; --f_BlockNum)
            {
                if(not (f_CurrentBlock = f_CurrentBlock->NextBlock))
                {
                    throw overflow_error("Tried to access index out of bounds on ChunkedPool!");
                }
            }
            // Add bounds checking for safety if needed
            return f_CurrentBlock->Slots[fp_Index];
        }

        template<typename... Args>
        void
            EmplaceBack(Args&&... fp_Args)
        {

        }

        //////////////////////////////////////////////
        // Allocation
        //////////////////////////////////////////////

        template<typename... Args>
        ValueType&
            Allocate(Handle& fp_OutHandle, Args&&... fp_Args)
        {
            // 1) Reuse from free list if possible
            if (not pm_FreeList.empty())
            {
                Handle f_Handle = pm_FreeList.back();
                pm_FreeList.pop_back();

                auto [f_BlockIndex, f_SlotIndex] = DecodeIndex(f_Handle.Index);

                Block* f_Block = pm_Blocks[f_BlockIndex].get();
                Slot& f_Slot = f_Block->Slots[f_SlotIndex];

                f_Slot.InUse = true;
                // bump generation to invalidate any older handles
                ++f_Slot.Generation;

                // placement-new into the existing storage
                f_Slot.Value = ValueType(forward<Args>(fp_Args)...);

                fp_OutHandle.Index = f_Handle.Index;
                fp_OutHandle.Generation = f_Slot.Generation;

                return f_Slot.Value;
            }

            // 2) Need a new slot at the end
            uint64_t f_GlobalIndex = NextGlobalIndex();

            auto [f_BlockIndex, f_SlotIndex] = DecodeIndex(f_GlobalIndex);

            EnsureBlockExists(f_BlockIndex);

            Block* f_Block = pm_Blocks[f_BlockIndex].get();
            Slot& f_Slot = f_Block->Slots[f_SlotIndex];

            f_Slot.InUse = true;
            // generation starts at 0, first use = 0
            f_Slot.Value = ValueType(forward<Args>(fp_Args)...);

            fp_OutHandle.Index = f_GlobalIndex;
            fp_OutHandle.Generation = f_Slot.Generation;

            return f_Slot.Value;
        }

        //////////////////////////////////////////////
        // Access
        //////////////////////////////////////////////

        [[nodiscard]] ValueType*
            GetPtr(const Handle& fp_Handle)
            noexcept
        {
            auto [f_BlockIndex, f_SlotIndex] = DecodeIndex(fp_Handle.Index);

            if (f_BlockIndex >= pm_Blocks.size())
            {
                return nullptr;
            }

            Block* f_Block = pm_Blocks[f_BlockIndex].get();
            Slot& f_Slot = f_Block->Slots[f_SlotIndex];

            if (not f_Slot.InUse or f_Slot.Generation != fp_Handle.Generation)
            {
                return nullptr;
            }

            return &f_Slot.Value;
        }

        [[nodiscard]] const ValueType*
            GetPtr(const Handle& fp_Handle)
            const noexcept
        {
            auto [f_BlockIndex, f_SlotIndex] = DecodeIndex(fp_Handle.Index);

            if (f_BlockIndex >= pm_Blocks.size())
            {
                return nullptr;
            }

            const Block* f_Block = pm_Blocks[f_BlockIndex].get();
            const Slot& f_Slot = f_Block->Slots[f_SlotIndex];

            if (not f_Slot.InUse or f_Slot.Generation != fp_Handle.Generation)
            {
                return nullptr;
            }

            return &f_Slot.Value;
        }

        [[nodiscard]] ValueType&
            Get(const Handle& fp_Handle)
        {
            ValueType* f_Ptr = GetPtr(fp_Handle);

            // You can replace this with debug-only assert if you prefer
            if (not f_Ptr)
            {
                throw runtime_error("ChunkedPool::Get() called with invalid or stale handle");
            }

            return *f_Ptr;
        }

        [[nodiscard]] const ValueType&
            Get(const Handle& fp_Handle)
            const
        {
            const ValueType* f_Ptr = GetPtr(fp_Handle);

            if (not f_Ptr)
            {
                throw runtime_error("ChunkedPool::Get() called with invalid or stale handle");
            }

            return *f_Ptr;
        }

        //////////////////////////////////////////////
        // Free
        //////////////////////////////////////////////

        void
            Free(const Handle& fp_Handle)
        {
            auto [f_BlockIndex, f_SlotIndex] = DecodeIndex(fp_Handle.Index);

            if (f_BlockIndex >= pm_Blocks.size())
            {
                return;
            }

            Block* f_Block = pm_Blocks[f_BlockIndex].get();
            Slot& f_Slot = f_Block->Slots[f_SlotIndex];

            if (not f_Slot.InUse or f_Slot.Generation != fp_Handle.Generation)
            {
                return; // stale or already free
            }

            f_Slot.InUse = false;
            ++f_Slot.Generation; // invalidate any existing handles

            pm_FreeList.push_back(Handle{ fp_Handle.Index, f_Slot.Generation });
        }

        //////////////////////////////////////////////
        // Introspection (optional)
        //////////////////////////////////////////////

        [[nodiscard]] uint64_t
            Capacity()
            const noexcept
        {
            return pm_Blocks.size() * BLOCK_SIZE;
        }

    private:
        static pair<uint64_t, uint64_t>
            DecodeIndex(uint64_t fp_GlobalIndex)
            noexcept
        {
            uint64_t f_BlockIndex = fp_GlobalIndex / BLOCK_SIZE;
            uint64_t f_SlotIndex = fp_GlobalIndex % BLOCK_SIZE;
            return { f_BlockIndex, f_SlotIndex };
        }

        [[nodiscard]] uint64_t
            NextGlobalIndex()
            noexcept
        {
            return pm_BackIndex++;
        }

        void
            EnsureBlockExists(uint64_t fp_BlockIndex)
        {
            while (pm_Blocks.size() <= fp_BlockIndex)
            {
                pm_Blocks.push_back(make_unique<Block>());
            }
        }
    };
}
