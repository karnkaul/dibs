#include <detail/glfw_instance.hpp>
#include <detail/log.hpp>
#include <instance_impl.hpp>

namespace dibs {
namespace detail {
using Type = Event::Type;

struct EventBuilder {
	Event operator()(int key, int scancode, int action, int mods) const noexcept {
		Event ret;
		ret.m_type = Type::eKey;
		ret.m_payload.key = {key, scancode, action, mods};
		return ret;
	}
	Event operator()(int button, int action, int mods) const noexcept {
		Event ret;
		ret.m_type = Type::eMouseButton;
		ret.m_payload.button = {button, action, mods};
		return ret;
	}
	Event operator()(std::uint32_t codepoint) const noexcept {
		Event ret;
		ret.m_type = Type::eText;
		ret.m_payload.u32 = codepoint;
		return ret;
	}
	Event operator()(Type const type, int const width, int const height) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.extent = {std::uint32_t(width), std::uint32_t(height)};
		return ret;
	}
	Event operator()(Type const type, double const x, double const y) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.mouseXY = {(float)x, (float)y};
		return ret;
	}
	Event operator()(Type const type, bool const b) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.b = b;
		return ret;
	}
};
} // namespace detail
} // namespace dibs

namespace dibs::detail {
namespace {
void onFocus(GLFWwindow* win, int entered) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eFocusChange, entered == GLFW_TRUE)); }
}

void onWindowResize(GLFWwindow* win, int width, int height) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eWindowResize, width, height)); }
}

void onFramebufferResize(GLFWwindow* win, int width, int height) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eFramebufferResize, width, height)); }
}

void onClose(GLFWwindow* win) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eClosed, true)); }
}

void onKey(GLFWwindow* win, int key, int scancode, int action, int mods) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(key, scancode, action, mods)); }
}

void onText(GLFWwindow* win, std::uint32_t scancode) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(scancode)); }
}

void onCursor(GLFWwindow* win, double x, double y) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eCursor, x, y)); }
}

void onMouseButton(GLFWwindow* win, int button, int action, int mods) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(button, action, mods)); }
}

void onScroll(GLFWwindow* win, double x, double y) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eScroll, x, y)); }
}

void onIconify(GLFWwindow* win, int iconified) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eIconify, iconified == GLFW_TRUE)); }
}

void onMaximize(GLFWwindow* win, int maximized) noexcept {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eMaximize, maximized == GLFW_TRUE)); }
}
} // namespace

Unique<GlfwInstance, GlfwInstance::Deleter> GlfwInstance::make() {
	if (glfwInit()) {
		glfwSetErrorCallback([](int code, char const* szDesc) { log("GLFW Error! [{}]: {}", code, szDesc); });
		return GlfwInstance{true};
	}
	return {};
}

void GlfwInstance::attachCallbacks(GLFWwindow* window) {
	glfwSetWindowFocusCallback(window, &onFocus);
	glfwSetWindowSizeCallback(window, &onWindowResize);
	glfwSetFramebufferSizeCallback(window, &onFramebufferResize);
	glfwSetWindowCloseCallback(window, &onClose);
	glfwSetKeyCallback(window, &onKey);
	glfwSetCharCallback(window, &onText);
	glfwSetCursorPosCallback(window, &onCursor);
	glfwSetMouseButtonCallback(window, &onMouseButton);
	glfwSetScrollCallback(window, &onScroll);
	glfwSetWindowIconifyCallback(window, &onIconify);
	glfwSetWindowMaximizeCallback(window, &onMaximize);
}

void GlfwInstance::detachCallbacks(GLFWwindow* window) {
	glfwSetWindowFocusCallback(window, {});
	glfwSetWindowSizeCallback(window, {});
	glfwSetFramebufferSizeCallback(window, {});
	glfwSetWindowCloseCallback(window, {});
	glfwSetKeyCallback(window, {});
	glfwSetCharCallback(window, {});
	glfwSetCursorPosCallback(window, {});
	glfwSetMouseButtonCallback(window, {});
	glfwSetScrollCallback(window, {});
	glfwSetWindowIconifyCallback(window, {});
	glfwSetWindowMaximizeCallback(window, {});
}
} // namespace dibs::detail
