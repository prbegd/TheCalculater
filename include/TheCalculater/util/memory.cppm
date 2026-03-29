/**
 * @file memory.cppm
 * @author prbegd
 * @date 2026-02-25
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include <concepts>
#include <functional>
#include <memory>
#include <utility>

export module TheCalculater.util.memory;

namespace TheCalculater::util {
    export template <typename T>
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
    export template <typename T>
    using observer_ptr = ObserverPtr<T>;

    export template <typename T>
    void swap(ObserverPtr<T>& lhs, ObserverPtr<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export template <typename T, typename U>
    constexpr bool operator==(const ObserverPtr<T>& lhs,
                              const ObserverPtr<U>& rhs) noexcept
    {
        return lhs.get() == rhs.get();
    }

    export template <typename T, typename U>
    constexpr bool operator!=(const ObserverPtr<T>& lhs,
                              const ObserverPtr<U>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    export template <typename T, typename U>
    constexpr bool operator<(const ObserverPtr<T>& lhs,
                             const ObserverPtr<U>& rhs) noexcept
    {
        return lhs.get() < rhs.get();
    }

    export template <typename T, typename U>
    constexpr bool operator<=(const ObserverPtr<T>& lhs,
                              const ObserverPtr<U>& rhs) noexcept
    {
        return !(rhs < lhs);
    }

    export template <typename T, typename U>
    constexpr bool operator>(const ObserverPtr<T>& lhs,
                             const ObserverPtr<U>& rhs) noexcept
    {
        return rhs < lhs;
    }

    export template <typename T, typename U>
    constexpr bool operator>=(const ObserverPtr<T>& lhs,
                              const ObserverPtr<U>& rhs) noexcept
    {
        return !(lhs < rhs);
    }

    export template <typename T>
    constexpr bool operator==(const ObserverPtr<T>& lhs,
                              std::nullptr_t) noexcept
    {
        return !lhs;
    }

    export template <typename T>
    constexpr bool operator==(std::nullptr_t,
                              const ObserverPtr<T>& rhs) noexcept
    {
        return !rhs;
    }

    export template <typename T>
    constexpr bool operator!=(const ObserverPtr<T>& lhs,
                              std::nullptr_t) noexcept
    {
        return static_cast<bool>(lhs);
    }

    export template <typename T>
    constexpr bool operator!=(std::nullptr_t,
                              const ObserverPtr<T>& rhs) noexcept
    {
        return static_cast<bool>(rhs);
    }

    // clang 我��你全家 这已经是我第二次因为clang编译器不支持的特性而改方案了
#ifdef __cpp_lib_atomic_shared_ptr
    export template <typename T>
    using AtomicSharedPtr = std::atomic<std::shared_ptr<T>>;
#else
    export template <typename T>
    class AtomicSharedPtr {
    private:
        std::shared_ptr<T> ptr;

    public:
        AtomicSharedPtr() = default;
        explicit AtomicSharedPtr(std::shared_ptr<T> p)
            : ptr(p)
        { }

        void store(const std::shared_ptr<T>& p, std::memory_order order = std::memory_order_seq_cst)
        {
            std::atomic_store_explicit(&ptr, p, order);
        }

        std::shared_ptr<T> load(std::memory_order order = std::memory_order_seq_cst) const
        {
            return std::atomic_load_explicit(&ptr, order);
        }
    };
#endif
} // namespace TheCalculater::util
export template <typename T>
struct std::hash<TheCalculater::util::ObserverPtr<T>> {
    size_t operator()(const TheCalculater::util::ObserverPtr<T>& p) const noexcept
    {
        return std::hash<typename TheCalculater::util::ObserverPtr<T>::TPtr> { }(p.get());
    }
};
