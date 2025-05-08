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

/*
this class is used for heap allocating Peach-E objects that can be easily tracked by the DebugManager, and the global allocation
tracker so that memory profiling becomes easier

It also contains useful tools for allocating memory on the heap in a controlled and somewhat efficient manner
*/

#include <unordered_map>
#include <iostream>

using namespace std;

#define pnew(T, ...) ::PeachCore::pstd::single_ptr<T>(new T(__VA_ARGS__), sizeof(T), __FILE__, __LINE__)

namespace PeachCore {
namespace pstd {

    struct AllocationRecord 
    {
        void* ptr;
        size_t size;
        const char* file;
        int line;
    };

    class AllocationTable 
    {
    public:
        void register_allocation(void* ptr, size_t size, const char* file, int line) 
        {
            allocations[ptr] = { ptr, size, file, line };
        }

        void deregister_allocation(void* ptr) 
        {
            allocations.erase(ptr);
        }

        void print_leaks() 
        {
            for (const auto& [ptr, record] : allocations)
            {
                cout << "[LEAK] " << record.size << " bytes at " << record.file << ":" << record.line << "\n";
            }
        }

    private:
        unordered_map<void*, AllocationRecord> allocations;
    };

    template<typename T>
    struct single_ptr 
    {
        T* ptr = nullptr;

        single_ptr() = default;

        explicit 
            single_ptr
            (
                T* raw, 
                size_t size, 
                const char* file, 
                int line
            )
        {
            ptr = raw;
        }

        ~single_ptr() 
        {
            reset();
        }

        single_ptr(single_ptr&& other) noexcept
        {
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        single_ptr& operator=(single_ptr&& other) noexcept
        {
            if (this != &other)
            {
                reset();
                ptr = other.ptr;
                other.ptr = nullptr;
            }

            return *this;
        }

        void 
            reset() 
        {
            if (ptr)
            {
                delete ptr;
                ptr = nullptr;
            }
        }

        template<typename T, typename... Args>
        single_ptr<T> make_single(Args&&... args)
        {
            return single_ptr<T>(new T(forward<Args>(args)...));
        }

        T* get() const noexcept { return ptr; }
        T& operator*() const { return *ptr; }
        T* operator->() const { return ptr; }

        // delete copy constructors
        single_ptr(const single_ptr&) = delete;
        single_ptr& operator=(const single_ptr&) = delete;
    };
} //namespace pstd
} //namespace PeachCore