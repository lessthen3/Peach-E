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
#include <atomic>

///PeachCore
#include "Transform.h"


namespace PeachCore::DoubleBuffered {

    struct UVState
    {
        float U1 = 0.0f;
        float V1 = 0.0f;
        float U2 = 1.0f;
        float V2 = 1.0f;
    };

    struct UVs 
    {
        UVState Slots[2];
        std::atomic<uint32_t> WriteIndex{ 0 };

        [[nodiscard]] UVState&
            GetWriteSlot()
            noexcept
        {
            return Slots[WriteIndex.load(std::memory_order_relaxed) & 1u];
        }

        [[nodiscard]] const UVState&
            GetReadSlot()
            const noexcept
        {
            return Slots[(WriteIndex.load(std::memory_order_acquire) + 1u) & 1u];
        }

        void
            CommitWrite()
            noexcept
        {
            WriteIndex.fetch_add(1u, std::memory_order_release);
        }

        void
            SetFrame(float fp_U1, float fp_V1, float fp_U2, float fp_V2)
            noexcept
        {
            UVState& f_Slot = GetWriteSlot();
            f_Slot.U1 = fp_U1;
            f_Slot.V1 = fp_V1;
            f_Slot.U2 = fp_U2;
            f_Slot.V2 = fp_V2;
        }
    };

    struct Transform2D
    {
    private:
        std::atomic<uint8_t> IsWritable = 0; //ik the alignment will still pad 4 bytes but this conveys intent better imo
    public:
        Math::Transform2D Slots[2];
        std::atomic<uint32_t> WriteIndex{ 0 }; // which slot game thread is writing

        void
            BeginWrite()
            noexcept
        {
            IsWritable.store(1u, std::memory_order_relaxed);
        }

        // Main thread: call at end of tick AFTER all transform writes for this frame are done
        // release ordering so render thread's acquire in GetReadSlot sees all prior writes
        void
            CommitWrite()
            noexcept
        {
            //make sure changes are set before read slot tries to access, otherwise it'll be reading stale data, and read matrices can't be updated via their regular paths owo
            GetWriteSlot().RegenerateLocalMatrix(); 
            WriteIndex.fetch_add(IsWritable.load(std::memory_order_relaxed), std::memory_order_release);
            IsWritable.store(0u, std::memory_order_relaxed); //unsigned 0 :O
        }

        [[nodiscard]] const mat4s&
            GetReadMatrix()
            const noexcept
        {
            return GetReadSlot().GetLocalMatrixNoRegenerate();
        }

        [[nodiscard]] vec2s
            GetPosition()
            const noexcept
        {
            return GetReadSlot().GetPosition();
        }

        [[nodiscard]] float
            GetRotation()
            const noexcept
        {
            return GetReadSlot().GetRotation();
        }

        [[nodiscard]] vec2s
            GetScale()
            const noexcept
        {
            return GetReadSlot().GetScale();
        }

    private:
        // Main thread: get the slot currently being written to
        [[nodiscard]] Math::Transform2D&
            GetWriteSlot()
            noexcept
        {
            return Slots[WriteIndex.load(std::memory_order_relaxed) & 1u];
        }

        // Render/physics thread: get the slot safe to read
        // Always the slot the main thread is NOT currently writing
        [[nodiscard]] const Math::Transform2D&
            GetReadSlot()
            const noexcept
        {
            return Slots[(WriteIndex.load(std::memory_order_acquire) + 1) & 1u];
        }
    };

    struct Transform3D
    {
        Math::Transform3D Slots[2];
        std::atomic<uint32_t> WriteIndex{ 0 };
    };
}