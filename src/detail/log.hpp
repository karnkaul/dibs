#pragma once
#include <ktl/str_format.hpp>
#include <iostream>

namespace dibs {
template <typename... Args>
void log(std::string_view fmt, Args const&... args) {
	std::cout << ktl::format(fmt, args...) << '\n';
}
} // namespace dibs
