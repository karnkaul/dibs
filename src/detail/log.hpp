#pragma once
#include <ktl/str_format.hpp>
#include <iostream>

namespace dibs {
constexpr bool trace_v =
#if defined(DIBS_DEBUG) && defined(DIBS_DEBUG_TRACE)
	true;
#else
	false;
#endif

template <typename... Args>
void log(std::string_view fmt, Args const&... args) {
	std::cout << ktl::str_format(fmt, args...) << '\n';
}

template <typename... Args>
void trace(std::string_view fmt, Args const&... args) {
	if constexpr (trace_v) { log(fmt, args...); }
}
} // namespace dibs
