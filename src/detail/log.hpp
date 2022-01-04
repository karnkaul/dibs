#pragma once
#include <ktl/str_format.hpp>
#include <iostream>

namespace dibs {
constexpr bool debug_v =
#if defined(DIBS_DEBUG)
	true;
#else
	false;
#endif

template <typename... Args>
void log(std::string_view fmt, Args const&... args) {
	std::cout << ktl::format(fmt, args...) << '\n';
}

template <typename... Args>
void trace(std::string_view fmt, Args const&... args) {
	if constexpr (debug_v) { log(fmt, args...); }
}
} // namespace dibs
