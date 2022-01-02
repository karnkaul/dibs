#pragma once
#include <ktl/expected.hpp>

namespace dibs {
enum class Error {
	eDuplicateInstance,
	eUnsupportedPlatform,
	eGlfwInitFailure,
	eWindowCreationFailure,
	eInvalidExtent,
};

template <typename T>
using Result = ktl::expected<T, Error>;
} // namespace dibs
