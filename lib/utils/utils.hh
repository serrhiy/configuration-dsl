#pragma once

#include <meta>
#include <string_view>

namespace utils
{

template <typename E>
    requires(std::meta::is_enum_type(^^E))
constexpr std::string_view GetEnumName(E value)
{
    constexpr static auto enumerators = std::define_static_array(enumerators_of(^^E));
    template for (constexpr auto enumerator : enumerators)
    {
        if (value != [:enumerator:])
            continue;
        return std::meta::identifier_of(enumerator);
    }
    return "<unknown>";
}

} // namespace utils