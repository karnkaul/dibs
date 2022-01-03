#pragma once
#include <cstdint>

namespace dibs {
template <typename T>
struct tvec2 {
	T x{}, y{};
};

using ivec2 = tvec2<std::int32_t>;
using uvec2 = tvec2<std::uint32_t>;
using vec2 = tvec2<float>;
} // namespace dibs
