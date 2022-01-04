#pragma once
#include <concepts>
#include <utility>

namespace dibs::detail {
template <typename T>
concept comparable = requires(T const& a, T const& b) {
	{ a == b } -> std::convertible_to<bool>;
};

template <comparable T, typename Deleter = void>
struct Unique {
	using type = T;
	using deleter_t = Deleter;

	T t;

	constexpr Unique(T t = T{}) noexcept : t(std::move(t)) {}
	constexpr Unique(Unique&& rhs) noexcept : Unique() { std::swap(t, rhs.t); }
	constexpr Unique& operator=(Unique rhs) noexcept { return (std::swap(t, rhs.t), *this); }
	constexpr ~Unique() noexcept requires(not std::same_as<Deleter, void>) {
		if (*this) { Deleter{}(t); }
	}

	constexpr explicit operator bool() const noexcept { return t != T{}; }
	bool operator==(Unique const&) const = default;
	constexpr T const& get() const noexcept { return t; }
	constexpr T& get() noexcept { return t; }
	constexpr operator T const&() const noexcept { return get(); }
	constexpr T const* operator->() const noexcept { return std::addressof(get()); }
};
} // namespace dibs::detail
