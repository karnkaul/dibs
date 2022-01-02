#include <detail/glfw_instance.hpp>
#include <detail/log.hpp>
#include <detail/vk_instance.hpp>
#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <cassert>

namespace dibs {
namespace {
struct Glfw {
	detail::UniqueGlfw instance;
	detail::UniqueWindow window;
};

Result<Glfw> makeGlfw(char const* title, uvec2 const extent, Instance::Flags const flags) noexcept {
	if (detail::g_window) { return Error::eDuplicateInstance; }
	if (extent.x == 0U || extent.y == 0U) { return Error::eInvalidArg; }
	auto instance = detail::GlfwInstance::make();
	if (!instance) { return Error::eGlfwInitFailure; }
	if (!glfwVulkanSupported()) { return Error::eUnsupportedPlatform; }
	auto window = instance->makeWindow(title, extent, flags);
	if (!window) { return Error::eWindowCreationFailure; }
	return Glfw{std::move(instance), std::move(window)};
}

bool centre(GLFWwindow* window) {
	auto const monitor = glfwGetPrimaryMonitor();
	if (!monitor) { return false; }
	auto const mode = glfwGetVideoMode(monitor);
	if (!mode) { return false; }
	int w{}, h{};
	glfwGetWindowSize(window, &w, &h);
	if (w <= 0 || h <= 0) { return false; }
	glfwSetWindowPos(window, (mode->width - w) / 2, (mode->height - h) / 2);
	return true;
}
} // namespace

using Clock = std::chrono::steady_clock;

struct Instance::Impl {
	Glfw glfw;
	detail::VKInstance vulkan;
	Clock::time_point elapsed = Clock::now();
};

Instance::Instance(std::unique_ptr<Impl>&& impl) noexcept : m_impl(std::move(impl)) {}
Instance::Instance(Instance&&) noexcept = default;
Instance& Instance::operator=(Instance&&) noexcept = default;
Instance::~Instance() noexcept {
	if (m_impl) { detail::g_window = {}; }
}

bool Instance::closing() const noexcept {
	assert(m_impl);
	return glfwWindowShouldClose(m_impl->glfw.window);
}

Time Instance::poll() noexcept {
	assert(m_impl);
	glfwPollEvents();
	auto const t = Clock::now();
	return t - std::exchange(m_impl->elapsed, t);
}

Result<Instance> Instance::Builder::operator()() const {
	auto glfw = makeGlfw(m_title.data(), m_extent, m_flags);
	if (!glfw) { return glfw.error(); }
	auto makeSurface = [&glfw](vk::Instance inst) {
		VkSurfaceKHR ret;
		glfwCreateWindowSurface(inst, glfw->window, nullptr, &ret);
		return vk::SurfaceKHR(ret);
	};
	auto vulkan = detail::VKInstance::make(std::move(makeSurface), detail::VKInstance::Flag::eValidation);
	if (!vulkan) { return vulkan.error(); }
	if (!centre(glfw->window)) { log("Failed to centre window"); }
	// all checks passed
	log("Using GPU: {}", vulkan->gpu.properties.deviceName);
	auto impl = std::make_unique<Instance::Impl>();
	impl->glfw = std::move(glfw).value();
	impl->vulkan = std::move(vulkan).value();
	if (!m_flags.test(Flag::eHidden)) { glfwShowWindow(impl->glfw.window); }
	return Instance(std::move(impl));
}
} // namespace dibs
