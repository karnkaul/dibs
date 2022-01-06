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

# vk-bootstrap
set(VK_BOOTSTRAP_VULKAN_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dyvk/include" CACHE STRING "" FORCE)
if(NOT EXISTS "${VK_BOOTSTRAP_VULKAN_HEADER_DIR}/vulkan/vulkan.h")
  message(FATAL_ERROR "Missing vulkan headers")
endif()

add_subdirectory(vk-bootstrap)
install(EXPORT vk-bootstrap-targets
  FILE vk-bootstrap-targets.cmake
  NAMESPACE vk-bootstrap::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
export(EXPORT vk-bootstrap-targets)

# Dear ImGui
add_subdirectory(dear_imgui)
