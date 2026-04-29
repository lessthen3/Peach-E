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

//WE WANT THIS CLASS TO PARSE THROUGH LOGS AND PROVIDE TOOLS TO INTROSPECT INTO THE ENGINE AND ITS ACTIVITIES

//THE SIMPLEST WAY I CAN THINK OF DOING THIS IS TO PROVIDE A WAY TO QUERY LOGS EFFICIENTLY AND POSSIBLY OTHER USEFUL DEBUGGING TOOLS

//FOR CREATING SANDBOX ENVIRONMENTS AND ISOLATING CERTAIN PARTS OF THE EXECUTION

#include <unordered_map>
#include <utility>


#ifdef PEACH_DEBUG

    #define PEACH_UNIQUE_PTR(T, ...) ::PeachCore::DebugUtils::CreateAndRegisterUniquePtr<T>(__FILE__, __LINE__, __VA_ARGS__)
    #define PEACH_SHARED_PTR(T, ...) ::PeachCore::DebugUtils::CreateAndRegisterSharedPtr<T>(__FILE__, __LINE__, __VA_ARGS__)
    #define TRACK_RAW_PTR(ptr)

    #define PEACH_NEW(T, ...) ::PeachCore::DebugUtils::CreateAndRegisterRawPtr<T>(__FILE__, __LINE__, __VA_ARGS__)
    #define PEACH_DELETE(ptr) ::PeachCore::DebugUtils::DeleteAndUnregisterRawPtr(ptr)

    #define PEACH_VECTOR(T) std::vector<T>
    #define PEACH_MAP(Tx, Ty) std::map<Tx, Ty>
    #define PEACH_UNMAP(Tx, Ty) std::unordered_map<Tx, Ty>

#else

    #define PEACH_UNIQUE_PTR(T, ...) make_unique<T>(__VA_ARGS__)
    #define PEACH_SHARED_PTR(T, ...) make_shared<T>(__VA_ARGS__)
    #define TRACK_RAW_PTR(ptr)

    #define PEACH_NEW(T, ...) new T(__VA_ARGS__)
    #define PEACH_DELETE(ptr) delete ptr

    #define PEACH_VECTOR(T) vector<T>
    #define PEACH_MAP(Tx, Ty) map<Tx, Ty>
    #define PEACH_UNMAP(Tx, Ty) unordered_map<Tx, Ty>

#endif

namespace PeachCore::DebugUtils {

    struct AllocationRecord 
    {
        void* ptr;
        size_t size;
        const char* file;
        int line;
    };

    struct AllocationTable //XXX: this class is used for each thread to track heap allocs, and maybe stack stuff over engine runtime uwu
    {
    public:

        // template<typename T, typename... Args>
        // unique_ptr<T>
        //     CreateAndRegisterUniquePtr(const char* fp_Filename, uint32_t fp_LineNumber, Args&&... args)
        // {
        //     size_t f_TypeSize = sizeof(T);

        //     //idk do some more stuff idec
        //    return make_unique<T>(forward<Args>(args)...);
        // }

        // template<typename T, typename... Args>
        // shared_ptr<T>
        //     CreateAndRegisterSharedPtr(const char* fp_Filename, uint32_t fp_LineNumber, Args&&... args)
        // {
        //     size_t f_TypeSize = sizeof(T);

        //     //idk do some more stuff idec
        //     return make_shared<T>(forward<Args>(args)...);
        // }

        // template<typename T, typename... Args>
        // T*
        //     CreateAndRegisterRawPtr(const char* fp_Filename, uint32_t fp_LineNumber, Args&&... args)
        // {
        //     size_t f_TypeSize = sizeof(T);

        //     //idk do some more stuff idec
        //     return new T(forward<Args>(args)...);
        // }

        // void
        //     DeleteAndUnregisterRawPtr(void* ptr)
        // {
        //     deregister_allocation(ptr);
        //     delete ptr;
        // }

        void register_allocation(void* ptr, size_t size, const char* file, int line) 
        {
            allocations[ptr] = { ptr, size, file, line };
        }

        void deregister_allocation(void* ptr) 
        {
            allocations.erase(ptr);
        }

        // void print_leaks() 
        // {
        //     for (const auto& [ptr, record] : allocations)
        //     {
        //         cout << "[LEAK] " << record.size << " bytes at " << record.file << ":" << record.line << "\n";
        //     }
        // }

        size_t
            GetTotalSizeOfAllocationTable()
            const
        {
            size_t f_SizeCounter = 0;

            for (const auto& [ptr, record] : allocations)
            {
                f_SizeCounter += record.size;
            }

            return f_SizeCounter;
        }

    private:
        std::unordered_map<void*, AllocationRecord> allocations;
    };

    // One table per thread, no locks required
    inline thread_local AllocationTable thread_alloc_table;

    // For convenience
    inline void* track_new(size_t size, const char* file, int line)
    {
        void* ptr = ::operator new(size);
        thread_alloc_table.register_allocation(ptr, size, file, line);
        return ptr;
    }

    inline void track_delete(void* ptr)
    {
        thread_alloc_table.deregister_allocation(ptr);
        ::operator delete(ptr);
    }

}//namespace PeachCore::DebugUtils