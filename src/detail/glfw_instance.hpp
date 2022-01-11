#pragma once

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>
#include <detail/unique.hpp>
#include <dibs/dibs.hpp>
#include <vector>

namespace dibs::detail {
struct GlfwInstance {
	bool init{};

	bool operator==(GlfwInstance const& rhs) const = default;

	struct Deleter {
		void operator()(GLFWwindow* window) const noexcept {
			detachCallbacks(window);
			glfwDestroyWindow(window);
		}
		void operator()(GlfwInstance) const noexcept {
			glfwSetErrorCallback(nullptr);
			glfwTerminate();
		}
	};

	static Unique<GlfwInstance, Deleter> make();

	static void attachCallbacks(GLFWwindow* window);
	static void detachCallbacks(GLFWwindow* window);

	Unique<GLFWwindow*, Deleter> makeWindow(char const* title, uvec2 extent, Instance::Flags flags) const noexcept {
		using Flag = Instance::Flag;
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_DECORATED, flags.test(Flag::eBorderless) ? 0 : 1);
		glfwWindowHint(GLFW_VISIBLE, 0);
		glfwWindowHint(GLFW_MAXIMIZED, flags.test(Flag::eMaximized) ? 1 : 0);
		auto ret = glfwCreateWindow(int(extent.x), int(extent.y), title, nullptr, nullptr);
		if (ret) { attachCallbacks(ret); }
		return ret;
	}
};

struct EventStorage;

struct GlfwData {
	GLFWwindow* window{};
	std::vector<Event>* events{};
	EventStorage* eventStorage{};
};

using UniqueGlfw = Unique<GlfwInstance, GlfwInstance::Deleter>;
using UniqueWindow = Unique<GLFWwindow*, GlfwInstance::Deleter>;

inline GlfwData g_glfwData;
} // namespace dibs::detail
