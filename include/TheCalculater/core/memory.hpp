/**
 * @file memory.hpp
 * @author prbegd
 * @date 2026-02-25
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once
#include <concepts>
#include <utility>

namespace TheCalculater {
    template <typename T>
    class ObserverPtr {
    public:
        using TPtr = T*;
        using TRef = T&;

        constexpr ObserverPtr() noexcept
            : ptr_(nullptr)
        { }

        constexpr ObserverPtr(std::nullptr_t) noexcept
            : ptr_(nullptr)
        { }

        explicit constexpr ObserverPtr(TPtr p) noexcept
            : ptr_(p)
        { }

        template <typename U>
            requires std::convertible_to<U*, TPtr>
        constexpr ObserverPtr(const ObserverPtr<U>& other) noexcept
            : ptr_(other.get())
        { }

        template <typename U, typename Deleter>
            requires std::convertible_to<U*, TPtr>
        explicit constexpr ObserverPtr(const std::unique_ptr<U, Deleter>& uother) noexcept
            : ptr_(uother.get())
        { }

        constexpr ObserverPtr& operator=(TPtr p) noexcept
        {
            ptr_ = p;
            return *this;
        }

        constexpr ObserverPtr& operator=(std::nullptr_t) noexcept
        {
            ptr_ = nullptr;
            return *this;
        }

        template <typename U>
            requires std::convertible_to<U*, TPtr>
        constexpr ObserverPtr& operator=(const ObserverPtr<U>& other) noexcept
        {
            ptr_ = other.get();
            return *this;
        }
        template <typename U, typename Deleter>
            requires std::convertible_to<U*, TPtr>
        constexpr ObserverPtr& operator=(const std::unique_ptr<U, Deleter>& uother) noexcept
        {
            ptr_ = uother.get();
            return *this;
        }

        constexpr void reset(TPtr p = nullptr) noexcept
        {
            ptr_ = p;
        }

        constexpr TPtr release() noexcept
        {
            TPtr p = ptr_;
            ptr_ = nullptr;
            return p;
        }

        constexpr void swap(ObserverPtr& other) noexcept
        {
            std::swap(ptr_, other.ptr_);
        }

        constexpr TPtr get() const noexcept { return ptr_; }

        constexpr explicit operator bool() const noexcept
        {
            return ptr_ != nullptr;
        }

        constexpr TRef operator*() const noexcept { return *ptr_; }

        constexpr TPtr operator->() const noexcept { return ptr_; }

    private:
        TPtr ptr_ = nullptr;
    };
    // Just for style consistent
    template <typename T>
    using observer_ptr = ObserverPtr<T>;

    template <typename T>
    void swap(ObserverPtr<T>& lhs, ObserverPtr<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <typename T, typename U>
    constexpr bool operator==(const ObserverPtr<T>& lhs,
        const ObserverPtr<U>& rhs) noexcept
    {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename U>
    constexpr bool operator!=(const ObserverPtr<T>& lhs,
        const ObserverPtr<U>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename T, typename U>
    constexpr bool operator<(const ObserverPtr<T>& lhs,
        const ObserverPtr<U>& rhs) noexcept
    {
        return lhs.get() < rhs.get();
    }

    template <typename T, typename U>
    constexpr bool operator<=(const ObserverPtr<T>& lhs,
        const ObserverPtr<U>& rhs) noexcept
    {
        return !(rhs < lhs);
    }

    template <typename T, typename U>
    constexpr bool operator>(const ObserverPtr<T>& lhs,
        const ObserverPtr<U>& rhs) noexcept
    {
        return rhs < lhs;
    }

    template <typename T, typename U>
    constexpr bool operator>=(const ObserverPtr<T>& lhs,
        const ObserverPtr<U>& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    template <typename T>
    constexpr bool operator==(const ObserverPtr<T>& lhs,
        std::nullptr_t) noexcept
    {
        return !lhs;
    }

    template <typename T>
    constexpr bool operator==(std::nullptr_t,
        const ObserverPtr<T>& rhs) noexcept
    {
        return !rhs;
    }

    template <typename T>
    constexpr bool operator!=(const ObserverPtr<T>& lhs,
        std::nullptr_t) noexcept
    {
        return static_cast<bool>(lhs);
    }

    template <typename T>
    constexpr bool operator!=(std::nullptr_t,
        const ObserverPtr<T>& rhs) noexcept
    {
        return static_cast<bool>(rhs);
    }
} // namespace TheCalculater
namespace std {
    template <typename T>
    struct hash<TheCalculater::ObserverPtr<T>> {
        size_t operator()(const TheCalculater::ObserverPtr<T>& p) const noexcept
        {
            return hash<typename TheCalculater::ObserverPtr<T>::pointer> {}(p.get());
        }
    };
}