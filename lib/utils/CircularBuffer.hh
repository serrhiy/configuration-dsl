#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <utility>

namespace utils
{

template <std::default_initializable T, std::size_t Capacity>
    requires(Capacity > 0)
class CircularBuffer final

{
    std::array<T, Capacity> buffer;
    std::size_t write_index;
    std::size_t count;

  public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = value_type &;
    using const_reference = const value_type &;

    constexpr CircularBuffer() : write_index{0}, count{0}
    {
    }

    constexpr void push_back(T value)
    {
        buffer[write_index] = std::move(value);
        write_index = (write_index + 1) % Capacity;
        count = std::min(count + 1, Capacity);
    }

    constexpr bool pop_back() noexcept
    {
        if (count == 0)
        {
            return false;
        }

        if (write_index == 0)
        {
            write_index = Capacity - 1;
        }
        else
        {
            write_index--;
        }
        count--;
        return true;
    }

    constexpr void clear() noexcept
    {
        write_index = 0;
        count = 0;
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return count;
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return count == 0;
    }

    [[nodiscard]] constexpr bool full() const noexcept
    {
        return count == Capacity;
    }

    [[nodiscard]] static constexpr size_type capacity() noexcept
    {
        return Capacity;
    }

    template <typename Self> [[nodiscard]] constexpr auto &&operator[](this Self &&self, size_type index) noexcept
    {
        assert(index < self.size());
        const std::size_t physical_index = (self.write_index + Capacity - 1 - index) % Capacity;
        return std::forward_like<Self>(self.buffer[physical_index]);
    }
};

} // namespace utils