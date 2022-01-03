#pragma once
#include <GLFW/glfw3.h>
#include <dibs/dibs.hpp>
#include <dibs/vk_types.hpp>

namespace dibs {
class Bridge {
  public:
	static VKDevice const& vulkan(Instance const& instance) noexcept;
	static GLFWwindow* glfw(Instance const& instance) noexcept;
	static vk::CommandBuffer drawCmd(Frame const& frame) noexcept;
};
} // namespace dibs
