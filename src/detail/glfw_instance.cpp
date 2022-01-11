#include <detail/glfw_instance.hpp>
#include <detail/log.hpp>
#include <instance_impl.hpp>

namespace dibs {
namespace detail {
using Type = Event::Type;

template <typename T>
struct Box {
	T t{};
};
template <typename T>
Box(T) -> Box<T>;

struct EventBuilder {
	Event operator()(Event::Key const key) const noexcept {
		Event ret;
		ret.m_type = Type::eKey;
		ret.m_payload.key = key;
		return ret;
	}
	Event operator()(Event::Button const button) const noexcept {
		Event ret;
		ret.m_type = Type::eMouseButton;
		ret.m_payload.button = button;
		return ret;
	}
	Event operator()(Box<std::uint32_t> codepoint) const noexcept {
		Event ret;
		ret.m_type = Type::eText;
		ret.m_payload.u32 = codepoint.t;
		return ret;
	}
	Event operator()(decltype(EventStorage::drops) const& drops) noexcept {
		Event ret;
		ret.m_type = Type::eFileDrop;
		ret.m_payload.index = drops.size() - 1U;
		return ret;
	}
	Event operator()(Type const type, uvec2 const uv2) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.uv2 = {uv2.x, uv2.y};
		return ret;
	}
	Event operator()(Type const type, ivec2 const iv2) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.iv2 = {iv2.x, iv2.y};
		return ret;
	}
	Event operator()(Type const type, dvec2 const dv2) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.dv2 = {dv2.x, dv2.y};
		return ret;
	}
	Event operator()(Type const type, Box<bool> const b) const noexcept {
		Event ret;
		ret.m_type = type;
		ret.m_payload.b = b.t;
		return ret;
	}
};
} // namespace detail
} // namespace dibs

namespace dibs::detail {
namespace {
EventStorage::Drop makeDrop(int count, char const** paths) {
	EventStorage::Drop ret;
	ret.reserve(std::size_t(count));
	for (int i = 0; i < count; ++i) { ret.push_back(paths[i]); }
	return ret;
}

void onClose(GLFWwindow* win) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eClosed, Box{true})); }
}

void onFocus(GLFWwindow* win, int entered) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eFocusChange, Box{entered == GLFW_TRUE})); }
}

void onCursorEnter(GLFWwindow* win, int entered) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eCursorEnter, Box{entered == GLFW_TRUE})); }
}

void onMaximize(GLFWwindow* win, int maximized) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eMaximize, Box{maximized == GLFW_TRUE})); }
}

void onIconify(GLFWwindow* win, int iconified) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eIconify, Box{iconified == GLFW_TRUE})); }
}

void onPos(GLFWwindow* win, int x, int y) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::ePosition, ivec2{x, y})); }
}

void onWindowResize(GLFWwindow* win, int width, int height) {
	if (win == g_glfwData.window && g_glfwData.events) {
		g_glfwData.events->push_back(EventBuilder{}(Event::Type::eWindowResize, uvec2{std::uint32_t(width), std::uint32_t(height)}));
	}
}

void onFramebufferResize(GLFWwindow* win, int width, int height) {
	if (win == g_glfwData.window && g_glfwData.events) {
		g_glfwData.events->push_back(EventBuilder{}(Event::Type::eFramebufferResize, uvec2{std::uint32_t(width), std::uint32_t(height)}));
	}
}

void onCursorPos(GLFWwindow* win, double x, double y) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eCursor, dvec2{x, y})); }
}

void onScroll(GLFWwindow* win, double x, double y) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Type::eScroll, dvec2{x, y})); }
}

void onKey(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Key{scancode, key, action, mods})); }
}

void onMouseButton(GLFWwindow* win, int button, int action, int mods) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Event::Button{button, action, mods})); }
}

void onText(GLFWwindow* win, std::uint32_t scancode) {
	if (win == g_glfwData.window && g_glfwData.events) { g_glfwData.events->push_back(EventBuilder{}(Box{scancode})); }
}

void onFileDrop(GLFWwindow* win, int count, char const** paths) {
	if (win == g_glfwData.window && g_glfwData.eventStorage && g_glfwData.events && g_glfwData.eventStorage->drops.has_space()) {
		g_glfwData.eventStorage->drops.push_back(makeDrop(count, paths));
		g_glfwData.events->push_back(EventBuilder{}(g_glfwData.eventStorage->drops));
	}
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
	glfwSetWindowCloseCallback(window, &onClose);
	glfwSetWindowFocusCallback(window, &onFocus);
	glfwSetCursorEnterCallback(window, &onCursorEnter);
	glfwSetWindowMaximizeCallback(window, &onMaximize);
	glfwSetWindowIconifyCallback(window, &onIconify);
	glfwSetWindowPosCallback(window, &onPos);
	glfwSetWindowSizeCallback(window, &onWindowResize);
	glfwSetFramebufferSizeCallback(window, &onFramebufferResize);
	glfwSetCursorPosCallback(window, &onCursorPos);
	glfwSetScrollCallback(window, &onScroll);
	glfwSetKeyCallback(window, &onKey);
	glfwSetMouseButtonCallback(window, &onMouseButton);
	glfwSetCharCallback(window, &onText);
	glfwSetDropCallback(window, &onFileDrop);
}

void GlfwInstance::detachCallbacks(GLFWwindow* window) {
	glfwSetWindowCloseCallback(window, {});
	glfwSetWindowFocusCallback(window, {});
	glfwSetCursorEnterCallback(window, {});
	glfwSetWindowMaximizeCallback(window, {});
	glfwSetWindowIconifyCallback(window, {});
	glfwSetWindowPosCallback(window, {});
	glfwSetWindowSizeCallback(window, {});
	glfwSetFramebufferSizeCallback(window, {});
	glfwSetCursorPosCallback(window, {});
	glfwSetScrollCallback(window, {});
	glfwSetKeyCallback(window, {});
	glfwSetMouseButtonCallback(window, {});
	glfwSetCharCallback(window, {});
	glfwSetDropCallback(window, {});
}
} // namespace dibs::detail
