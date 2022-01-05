#include <detail/glfw_instance.hpp>
#include <detail/log.hpp>
#include <instance_impl.hpp>

namespace dibs::detail {
namespace {
void onFocus(GLFWwindow* win, int entered) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->focus(entered == 1); }
}

void onWindowResize(GLFWwindow* win, int width, int height) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->windowResize({std::uint32_t(width), std::uint32_t(height)}); }
}

void onFramebufferResize(GLFWwindow* win, int width, int height) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->framebufferResize({std::uint32_t(width), std::uint32_t(height)}); }
}

void onClose(GLFWwindow* win) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->closed(); }
}

void onKey(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->key(KeyEvent{key, scancode, action, mods}); }
}

void onText(GLFWwindow* win, std::uint32_t scancode) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->text(scancode); }
}

void onCursor(GLFWwindow* win, double x, double y) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->cursor({x, y}); }
}

void onMouseButton(GLFWwindow* win, int button, int action, int mods) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->mouseButton({button, action, mods}); }
}

void onScroll(GLFWwindow* win, double x, double y) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->scroll({x, y}); }
}

void onIconify(GLFWwindow* win, int iconified) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->iconified(iconified == 1); }
}

void onMaximize(GLFWwindow* win, int maximized) {
	if (win == g_glfwData.window && g_glfwData.delegates) { g_glfwData.delegates->maximized(maximized == 1); }
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
