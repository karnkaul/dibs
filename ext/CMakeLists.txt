# glfw
set(GLFW_VULKAN_STATIC OFF)
set(GLFW_INSTALL ${DIBS_INSTALL})
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)
if(DIBS_INSTALL)
  export(EXPORT glfwTargets)
endif()

# dyvk
set(DYVK_INSTALL ${DIBS_INSTALL})
add_subdirectory(dyvk)

# vk-bootstrap
set(VK_BOOTSTRAP_VULKAN_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dyvk/include" CACHE STRING "" FORCE)
if(NOT EXISTS "${VK_BOOTSTRAP_VULKAN_HEADER_DIR}/vulkan/vulkan.h")
  message(FATAL_ERROR "Missing vulkan headers")
endif()

add_subdirectory(vk-bootstrap)
if(DIBS_INSTALL)
  install(EXPORT vk-bootstrap-targets
    FILE vk-bootstrap-targets.cmake
    NAMESPACE vk-bootstrap::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )
  export(EXPORT vk-bootstrap-targets)
endif()

# Dear ImGui
add_subdirectory(dear_imgui)
