#include <detail/glfw_instance.hpp>
#include <detail/log.hpp>
#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <cassert>

namespace dibs {
namespace {
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
	detail::UniqueGlfw glfw;
	detail::UniqueWindow window;
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
	return glfwWindowShouldClose(m_impl->window);
}

Time Instance::poll() noexcept {
	assert(m_impl);
	glfwPollEvents();
	auto const t = Clock::now();
	return t - std::exchange(m_impl->elapsed, t);
}

Result<Instance> Instance::Builder::operator()() const {
	if (detail::g_window) { return Error::eDuplicateInstance; }
	if (m_extent.x == 0U || m_extent.y == 0U) { return Error::eInvalidExtent; }
	auto glfwInst = detail::GlfwInstance::make();
	if (!glfwInst) { return Error::eGlfwInitFailure; }
	// TODO: after integrating dyvk
	// if (!glfwVulkanSupported()) { return Error::eUnsupportedPlatform; }
	auto window = glfwInst->makeWindow(m_title.data(), m_extent, m_flags);
	if (!window) { return Error::eWindowCreationFailure; }
	if (!centre(window)) { log("Failed to centre window"); }
	// all checks passed, set globals
	auto impl = std::make_unique<Instance::Impl>();
	impl->glfw = std::move(glfwInst);
	impl->window = std::move(window);
	if (!m_flags.test(Flag::eHidden)) { glfwShowWindow(impl->window); }
	return Instance(std::move(impl));
}
} // namespace dibs
