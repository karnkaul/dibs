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

template <typename T>
constexpr tvec2<T> operator+(tvec2<T> a, tvec2<T> b) noexcept {
	return {a.x + b.x, a.y + b.y};
}

template <typename T>
constexpr tvec2<T> operator-(tvec2<T> a, tvec2<T> b) noexcept {
	return {a.x - b.x, a.y - b.y};
}

template <typename T>
constexpr tvec2<T> operator*(tvec2<T> a, tvec2<T> b) noexcept {
	return {a.x * b.x, a.y * b.y};
}

template <typename T>
constexpr tvec2<T> operator/(tvec2<T> a, tvec2<T> b) noexcept {
	return {a.x / b.x, a.y / b.y};
}
} // namespace dibs
