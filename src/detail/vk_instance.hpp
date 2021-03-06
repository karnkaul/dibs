#pragma once
#include <dibs/bridge.hpp>
#include <dibs/error.hpp>
#include <ktl/async/kfunction.hpp>
#include <ktl/enum_flags/enum_flags.hpp>

namespace dibs::detail {
using MakeSurface = ktl::kfunction<vk::SurfaceKHR(vk::Instance)>;

struct VKInstance {
	enum class Flag { eValidation };
	using Flags = ktl::enum_flags<Flag, std::uint8_t>;

	vk::UniqueInstance instance;
	vk::UniqueDebugUtilsMessengerEXT messenger;
	VKGpu gpu;
	vk::UniqueDevice device;
	vk::UniqueSurfaceKHR surface;
	VKQueue queue;

	static Result<VKInstance> make(MakeSurface makeSurface, Flags flags);
};
} // namespace dibs::detail
