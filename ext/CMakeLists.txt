# ktl
add_subdirectory(ktl)

# glfw
set(GLFW_VULKAN_STATIC OFF)
set(GLFW_INSTALL ON)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)
export(EXPORT glfwTargets)

# dyvk
add_subdirectory(dyvk)
