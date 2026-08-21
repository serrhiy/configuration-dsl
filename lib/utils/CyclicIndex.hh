#pragma once

#include <cstddef>

template <std::size_t Cycle>
    requires(Cycle > 0)
class CyclicIndex final
{
    std::size_t index;

  public:
    constexpr CyclicIndex() noexcept : index{0}
    {
    }

    constexpr CyclicIndex &operator++() noexcept
    {
        index = (index + 1) % Cycle;
        return *this;
    }

    constexpr CyclicIndex operator++(int) noexcept
    {
        CyclicIndex old = *this;
        this->operator++();
        return old;
    }

    constexpr CyclicIndex &operator--() noexcept
    {
        index = (index + Cycle - 1) % Cycle;
        return *this;
    }

    constexpr CyclicIndex operator--(int) noexcept
    {
        CyclicIndex old = *this;
        this->operator--();
        return old;
    }

    [[nodiscard]] constexpr std::size_t GetIndex() const noexcept
    {
        return index;
    }

    constexpr CyclicIndex &operator=(std::size_t value) noexcept
    {
        index = value;
        return *this;
    }
};
