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

#include <array>
#include <utility>
#include <stdexcept>

namespace PeachCore {

    using namespace std;

    template<typename T, size_t pm_MaxCapacity>
    class RingBuffer 
    {
        static_assert(pm_MaxCapacity > 0, "RingBuffer size must be greater than 0");

    public:
        constexpr RingBuffer() = default;

        constexpr void 
            Clear() 
            noexcept 
        {
            pm_Head = pm_Size = 0;
        }

        [[nodiscard]] bool 
            TryPush(const T& fp_Val)
        {
            if (IsFull())
            {
                return false;
            }          

            PushOverwrite(fp_Val); // Safe push, now guaranteed to not assert

            return true;
        }

        [[nodiscard]] bool 
            TryPush(T&& fp_Val)
        {
            if (IsFull())
            {
                return false;
            }

            PushOverwrite(std::move(fp_Val)); // Move into buffer

            return true;
        }

        template<typename... Args>
        [[nodiscard]] bool
            TryEmplace(Args&&... fp_Args)
        {
            if (IsFull())
            {
                return false;
            }

            EmplaceOverwrite(std::forward<Args>(fp_Args)...);

            return true;
        }

        constexpr void 
            Push(const T& fp_Val)
        {
            PushOverwrite(fp_Val);
        }

        constexpr void 
            Push(T&& fp_Val)
        {
            PushOverwrite(std::move(fp_Val));
        }

        template<typename... Args>
        constexpr void 
            Emplace(Args&&... fp_Args)
        {
            EmplaceOverwrite(std::forward<Args>(fp_Args)...);
        }

        void 
            Pop() 
        {
            if (IsEmpty())
            {
                throw underflow_error("Cannot pop from empty RingBuffer");
            }

            // We remove the oldest element, oldest is at FrontIndex() so we just reduce size.
            --pm_Size;
        }

        [[nodiscard]] T&
            Front()
        {
            if (IsEmpty())
            {
                throw underflow_error("Cannot access front of empty RingBuffer");
            }

            return pm_Buffer[FrontIndex()];
        }

        [[nodiscard]] const T&
            Front()
            const 
        {
            if (IsEmpty())
            {
                throw underflow_error("Cannot access front of empty RingBuffer");
            }

            return pm_Buffer[FrontIndex()];
        }

        [[nodiscard]] T& 
            Back()
        {
            if (IsEmpty())
            {
                throw out_of_range("RingBuffer::Back: buffer is empty");
            }

            return pm_Buffer[BackIndex()];
        }
        
        [[nodiscard]] const T& 
            Back()
            const
        {
            if (IsEmpty())
            {
                throw out_of_range("RingBuffer::Back: buffer is empty");
            }

            return pm_Buffer[BackIndex()];
        }

        [[nodiscard]] T& 
            At(size_t fp_Index)
        {
            if (fp_Index >= pm_Size)
            {
                throw out_of_range("RingBuffer::At: index out of range");
            }

            return pm_Buffer[(FrontIndex() + fp_Index) % pm_MaxCapacity];
        }

        [[nodiscard]] const T& 
            At(size_t fp_Index)
            const 
        {
            if (fp_Index >= pm_Size)
            {
                throw out_of_range("RingBuffer::At: index out of range");
            }

            return pm_Buffer[(FrontIndex() + fp_Index) % pm_MaxCapacity];
        }

        [[nodiscard]] constexpr size_t 
            CurrentSize() 
            const noexcept 
        {
            return pm_Size;
        }

        [[nodiscard]] constexpr size_t 
            MaxCapacity() 
            const noexcept 
        {
            return pm_MaxCapacity;
        }

        [[nodiscard]] constexpr bool 
            IsEmpty() 
            const noexcept 
        {
            return pm_Size == 0;
        }

        [[nodiscard]] constexpr bool 
            IsFull() 
            const noexcept 
        {
            return pm_Size == pm_MaxCapacity;
        }

    private:
        array<T, pm_MaxCapacity> pm_Buffer{};
        size_t pm_Head = 0;   // index of next write
        size_t pm_Size = 0;   // number of valid elements

    private:
        ////////////////// helpers //////////////////

        [[nodiscard]] size_t
            FrontIndex() const noexcept
        {
            // Oldest element
            return (pm_Head + pm_MaxCapacity - pm_Size) % pm_MaxCapacity;
        }

        [[nodiscard]] size_t
            BackIndex() const noexcept
        {
            // Most recently inserted element
            return (pm_Head + pm_MaxCapacity - 1) % pm_MaxCapacity;
        }

        void
            AdvanceHead() noexcept
        {
            pm_Head = (pm_Head + 1) % pm_MaxCapacity;
            if (pm_Size < pm_MaxCapacity)
            {
                ++pm_Size;
            }
            // If already full, we overwrote the oldest; size stays at capacity.
        }

        void
            PushOverwrite(const T& fp_Val)
        {
            pm_Buffer[pm_Head] = fp_Val;
            AdvanceHead();
        }

        void
            PushOverwrite(T&& fp_Val)
        {
            pm_Buffer[pm_Head] = std::move(fp_Val);
            AdvanceHead();
        }

        template<typename... Args>
        void
            EmplaceOverwrite(Args&&... fp_Args)
        {
            pm_Buffer[pm_Head] = T(std::forward<Args>(fp_Args)...);
            AdvanceHead();
        }
    };
} // namespace PeachCore
