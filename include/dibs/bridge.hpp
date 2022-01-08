#pragma once
#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>
#include <dibs/dibs.hpp>

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

class Bridge {
  public:
	static VKDevice const& vulkan(Instance const& instance) noexcept;
	static GLFWwindow* glfw(Instance const& instance) noexcept;
	static vk::CommandBuffer drawCmd(Frame const& frame) noexcept;
};
} // namespace dibs
