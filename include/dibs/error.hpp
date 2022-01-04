#pragma once
#include <ktl/expected.hpp>

namespace dibs {
enum class Error {
	eDuplicateInstance,
	eUnsupportedPlatform,
	eGlfwInitFailure,
	eVulkanInitFailure,
	eWindowCreationFailure,
	eInvalidArg,
};

template <typename T>
using Result = ktl::expected<T, Error>;
} // namespace dibs
