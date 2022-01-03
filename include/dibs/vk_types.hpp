#pragma once
#include <vulkan/vulkan.hpp>

namespace dibs {
struct VKGpu {
	vk::PhysicalDeviceProperties properties;
	std::vector<vk::SurfaceFormatKHR> formats;
	vk::PhysicalDevice device;
};

struct VKQueue {
	vk::Queue queue;
	std::uint32_t family{};
};

struct VKDevice {
	VKGpu gpu;
	vk::Instance instance;
	vk::Device device;
	VKQueue queue;
};
} // namespace dibs
