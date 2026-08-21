#pragma once

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include "CyclicIndex.hh"

namespace utils
{

template <typename T, std::size_t Capacity>
    requires(Capacity > 0)
class CircularBuffer final
{
    template <typename U> union Slot {
        U value;

        constexpr Slot() noexcept
        {
        }

        constexpr ~Slot()
            requires std::is_trivially_destructible_v<U>
        = default;

        constexpr ~Slot() noexcept
        {
        }
    };

    std::array<Slot<T>, Capacity> slots;
    CyclicIndex<Capacity> write_index;
    std::size_t count;

    [[nodiscard]] constexpr std::size_t physical_index(std::size_t index) const noexcept
    {
        return (write_index.GetIndex() + Capacity - 1 - index) % Capacity;
    }

  public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = value_type &;
    using const_reference = const value_type &;

    constexpr CircularBuffer() : slots{}, write_index{}, count{0}
    {
    }

    constexpr ~CircularBuffer()
        requires std::is_trivially_destructible_v<T>
    = default;

    constexpr ~CircularBuffer()
    {
        clear();
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return count;
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    [[nodiscard]] constexpr bool full() const noexcept
    {
        return size() == Capacity;
    }

    [[nodiscard]] static constexpr size_type capacity() noexcept
    {
        return Capacity;
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    constexpr reference emplace_back(Args &&...args)
    {
        Slot<T> &slot = slots[write_index.GetIndex()];
        if (full())
        {
            std::destroy_at(std::addressof(slot.value));
            --count;
        }
        std::construct_at(std::addressof(slot.value), std::forward<Args>(args)...);
        ++count;
        ++write_index;
        return slot.value;
    }

    constexpr void push_back(T value)
        requires std::move_constructible<T>
    {
        emplace_back(std::move(value));
    }

    constexpr bool pop_back() noexcept
    {
        if (empty())
        {
            return false;
        }
        --write_index;
        --count;
        std::destroy_at(std::addressof(slots[write_index.GetIndex()].value));
        return true;
    }

    constexpr void clear() noexcept
    {
        for (size_type index = 0; index < size(); ++index)
        {
            std::destroy_at(std::addressof(slots[physical_index(index)].value));
        }
        count = 0;
        write_index = 0;
    }

    template <typename Self> [[nodiscard]] constexpr auto &&operator[](this Self &&self, size_type index) noexcept
    {
        assert(index < self.size());
        return std::forward_like<Self>(self.slots[self.physical_index(index)].value);
    }
};

} // namespace utils