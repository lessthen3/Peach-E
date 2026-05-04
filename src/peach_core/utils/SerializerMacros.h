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

/// Magic World

// One field: expands to v.field("a", a) inside peachVisit
#define PEACH_FIELD(f) v(#f, f)

// Base / non-inherited type
#define PEACH_SERIALIZABLE(...)                           \
    using peach_serializable_tag = void;                  \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) {                             \
        __VA_ARGS__;                                      \
    }                                                     \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) const {                       \
        __VA_ARGS__;                                      \
    }

// Derived type that wants to also serialize its Base part
#define PEACH_SERIALIZABLE_DERIVED(Base, ...)             \
    using peach_serializable_tag = void;                  \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) {                             \
        Base::PEACH_VISIT(v);                             \
        __VA_ARGS__;                                      \
    }                                                     \
    template <typename V>                                 \
    void PEACH_VISIT(V&& v) const {                       \
        Base::PEACH_VISIT(v);                             \
        __VA_ARGS__;                                      \
    }
