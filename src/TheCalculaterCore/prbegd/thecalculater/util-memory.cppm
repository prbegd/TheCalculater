/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include <version>

export module prbegd.thecalculater.util:memory;
import std;
import :exceptions;

namespace thecalculater::util {
export template <typename T>
class unique_pmr_ptr;

export template <typename T>
struct PmrDeleter {
    template <typename U>
    constexpr PmrDeleter(const PmrDeleter<U>&) noexcept
    {
        static_assert(std::convertible_to<U*, T*> && (std::is_polymorphic_v<T> || std::same_as<T, U>), "Can't cast non-polymorphic type U to T due to potential object slicing issue. Are you trying to cast from non-polymorphic derived type to its base type?");
    }

    constexpr PmrDeleter(const PmrDeleter&) = default;
    constexpr PmrDeleter(PmrDeleter&&) noexcept = default;
    constexpr PmrDeleter& operator=(const PmrDeleter&) = default;
    constexpr PmrDeleter& operator=(PmrDeleter&&) noexcept = default;
    constexpr ~PmrDeleter() = default;

    void operator()(T* ptr) const
    {
        static_assert(!std::is_void_v<T>, "Can't delete pointer to incomplete type");
        static_assert(sizeof(T) > 0, "Can't delete pointer to incomplete type"); // NOLINT

        void* body = nullptr;
        if constexpr (std::is_polymorphic_v<T>) {
            body = dynamic_cast<void*>(ptr);

            static_assert(std::has_virtual_destructor_v<T>, "Polymorphic type T must have a virtual destructor for proper deallocation.");
        } else {
            body = static_cast<void*>(ptr);
        }
        if (!body) {
            return;
        }
        auto* header = reinterpret_cast<Header*>(static_cast<std::byte*>(body) - sizeof(Header));
        std::size_t padding = (header->align - (sizeof(Header) % header->align)) % header->align;
        void* fullBlock = static_cast<std::byte*>(body) - sizeof(Header) - padding;
        std::pmr::memory_resource* resource = header->resource;
        std::size_t size = header->size;
        std::size_t align = header->align;

        ptr->~T();
        if constexpr (!std::is_trivial_v<Header>) {
            static_cast<Header*>(header)->~Header();
        }

        resource->deallocate(reinterpret_cast<std::byte*>(fullBlock), size, align);
    }

    struct Header {
        std::pmr::memory_resource* resource;
        std::size_t size;
        std::size_t align;
    };

private:
    PmrDeleter() noexcept = default;
    template <typename U, typename... Args>
    friend unique_pmr_ptr<U> makeUniquePmr(std::pmr::memory_resource*, Args&&...);

    friend class unique_pmr_ptr<T>;
};

export template <typename T>
class unique_pmr_ptr : public std::unique_ptr<T, PmrDeleter<T>> {
public:
    constexpr unique_pmr_ptr() noexcept
        : std::unique_ptr<T, PmrDeleter<T>>(nullptr, { })
    { }
    using std::unique_ptr<T, PmrDeleter<T>>::unique_ptr;
};

export template <typename T, typename... Args>
unique_pmr_ptr<T> makeUniquePmr(std::pmr::memory_resource* resource, Args&&... args)
{
    if (!resource) {
        throwext(std::invalid_argument("Memory resource pointer cannot be null."));
    }
    using Header = unique_pmr_ptr<T>::deleter_type::Header;
    std::size_t align = std::max(alignof(T), alignof(Header));
    std::size_t padding = (align - (sizeof(Header) % align)) % align;
    std::size_t size = padding + sizeof(T) + sizeof(Header);
    void* block = resource->allocate(size, align);
    void* header = static_cast<std::byte*>(block) + padding;
    void* body = static_cast<std::byte*>(block) + padding + sizeof(Header);
    try {
        new (header) Header { resource, size, align };
        return unique_pmr_ptr<T>(::new (body) T(std::forward<Args>(args)...), PmrDeleter<T> { });
    } catch (...) {
        if constexpr (!std::is_trivial_v<Header>) {
            static_cast<Header*>(header)->~Header();
        }
        resource->deallocate(static_cast<std::byte*>(block), size, align);
        throw;
    }
}
export template <typename T, typename... Args>
constexpr unique_pmr_ptr<T> makeUniquePmr(std::nullptr_t, Args&&...) = delete;

export template <typename T>
constexpr std::pmr::memory_resource* ownerOf(const unique_pmr_ptr<T>& ptr)
{
    using Header = unique_pmr_ptr<T>::deleter_type::Header;
    if (!ptr) {
        throwext(std::invalid_argument("Null pointer does not have a corresponding memory resource owner."));
    }
    void* body = nullptr;
    if constexpr (std::is_polymorphic_v<T>) {
        body = dynamic_cast<void*>(ptr.get());
    } else {
        body = static_cast<void*>(ptr.get());
    }
    auto* header = reinterpret_cast<Header*>(static_cast<std::byte*>(body) - sizeof(Header));
    return header->resource;
}

export template <typename T>
constexpr std::shared_ptr<T> wrapUnownedAsShared(T* ptr)
{
    return { ptr, [](T*) { } };
}

#ifdef __cpp_lib_atomic_shared_ptr
export template <typename T>
using atomic_shared_ptr = std::atomic<std::shared_ptr<T>>;
#else
export template <typename T>
class atomic_shared_ptr {
private:
    std::shared_ptr<T> ptr;

public:
    constexpr atomic_shared_ptr() noexcept = default;
    explicit atomic_shared_ptr(std::shared_ptr<T> p)
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
} // namespace thecalculater::util