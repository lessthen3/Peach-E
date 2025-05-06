/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - 🍑 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                     Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <array>
#include <cstddef>
#include <cassert>
#include <utility>

using namespace std;

namespace PeachCore {

    template<typename T, size_t pm_MaxCapacity>
    class RingBuffer 
    {
        static_assert(pm_MaxCapacity > 0, "RingBuffer size must be greater than 0");

    public:
        constexpr RingBuffer() = default;

        constexpr void Clear() noexcept 
        {
            pm_Head = pm_Tail = pm_Size = 0;
        }

        constexpr void SafePush(const T& value)
        {
            assert(not IsFull() and "RingBuffer overflow (consider overwriting or increasing capacity)");
            Push(value); // Safe push, now guaranteed to not assert
        }

        constexpr void SafePush(T&& value) 
        {
            assert(not IsFull() and "RingBuffer overflow (consider overwriting or increasing capacity)");
            Push(move(value)); // Move into buffer
        }

        constexpr void ForcePush(const T& value)
        {
            if (IsFull()) 
            {
                Pop(); // Drop the oldest value to make space
            }

            Push(value); // Safe push, now guaranteed to not assert
        }

        constexpr void ForcePush(T&& value)
        {
            if (IsFull()) 
            {
                Pop(); // Drop the oldest value
            }

            Push(move(value)); // Move into buffer
        }

        constexpr void Pop() 
        {
            assert(not IsEmpty() and "Cannot pop from empty RingBuffer");
            pm_Tail = (pm_Tail + 1) % pm_MaxCapacity;
            --pm_Size;
        }

        [[nodiscard]] constexpr T& Front()
        {
            assert(not IsEmpty() and "Cannot access front of empty RingBuffer");
            return pm_Buffer[pm_Tail];
        }

        [[nodiscard]] constexpr const T& Front() const 
        {
            assert(not IsEmpty() and "Cannot access front of empty RingBuffer");
            return pm_Buffer[pm_Tail];
        }

        [[nodiscard]] constexpr T& At(size_t index)
        {
            assert(index < pm_Size and "Index out of bounds");
            return pm_Buffer[(pm_Tail + index) % pm_MaxCapacity];
        }

        [[nodiscard]] constexpr const T& At(size_t index) const 
        {
            assert(index < pm_Size and "Index out of bounds");
            return pm_Buffer[(pm_Tail + index) % pm_MaxCapacity];
        }

        [[nodiscard]] constexpr size_t CurrentSize() const noexcept 
        {
            return pm_Size;
        }

        [[nodiscard]] constexpr size_t MaxCapacity() const noexcept 
        {
            return pm_MaxCapacity;
        }

        [[nodiscard]] constexpr bool IsEmpty() const noexcept 
        {
            return pm_Size == 0;
        }

        [[nodiscard]] constexpr bool IsFull() const noexcept 
        {
            return pm_Size == pm_MaxCapacity;
        }

    private:
        array<T, pm_MaxCapacity> pm_Buffer{};
        size_t pm_Head = 0;
        size_t pm_Tail = 0;
        size_t pm_Size = 0;

    private:
        constexpr void Push(const T& value)
        {
            pm_Buffer[pm_Head] = value;
            pm_Head = (pm_Head + 1) % pm_MaxCapacity;
            ++pm_Size;
        }

        constexpr void Push(T&& value)
        {
            pm_Buffer[pm_Head] = move(value);
            pm_Head = (pm_Head + 1) % pm_MaxCapacity;
            ++pm_Size;
        }
    };
} // namespace PeachCore
