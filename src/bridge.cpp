#include <detail/expect.hpp>
#include <dibs/bridge.hpp>
#include <instance_impl.hpp>

namespace dibs {
VKDevice const& Bridge::vulkan(Instance const& instance) noexcept {
	EXPECT(instance.m_impl);
	return instance.m_impl->device;
}

GLFWwindow* Bridge::glfw(Instance const& instance) noexcept {
	EXPECT(instance.m_impl);
	return instance.m_impl->glfw.window;
}

vk::CommandBuffer Bridge::drawCmd(Frame const& frame) noexcept {
	EXPECT(frame.m_instance.m_impl->acquired);
	return frame.m_instance.m_impl->frameSync.get().cb;
}
} // namespace dibs
