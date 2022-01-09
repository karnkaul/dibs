# Dear ImGui BootStrap (dibs)

A small quick-start library that provides a GLFW / Vulkan backend for Dear ImGui.

[![Build Status](https://github.com/karnkaul/dibs/actions/workflows/ci.yml/badge.svg)](https://github.com/karnkaul/dibs/actions/workflows/ci.yml)

## Features

- Lightweight wrapper with minimal bloat
- Create a GLFW window, Vulkan instance, device, and swapchain in one call
- Start a new frame with a clear colour in one call
- Reuse a single install across multiple CMake projects

## Usage

Add project to build tree if building from source, or installed package path to `CMAKE_PREFIX_PATH`, and link to `dibs::dibs`.

```cmake
# build source
add_subdirectory(path/to/dibs)

# imported package
find_package(dibs REQUIRED)

add_executable(foo)
target_link_libraries(foo PRIVATE dibs::dibs)
```

Include `dibs/dibs.hpp` and `imgui.h` and create a `dibs::Instance` to initialize `GLFW`, `Vulkan`, `Dear ImGui`, and create a window.

Refer to [the wiki](https://github.com/karnkaul/dibs/wiki) for more information.

### Example

Refer to [example.cpp](examples/example.cpp).

### Misc

1. Do not use branch names with `FetchContent` - as source branches change, target builds / older commits will break
1. Only one `dibs::Instance` is supported; `Dear ImGui` uses global state and does not directly support multiple windows
1. `dibs` links to Vulkan (headers) and GLFW publicly; user code can reference those libraries if desired
    1. However, `dibs.hpp` is designed to be lightweight, and does not include Vulkan / GLFW headers
    1. To extract such types from `dibs::Instance` (eg `GLFWwindow*`, `vk::CommandBuffer`), use `dibs/bridge.hpp` (not demonstrated)

## External Dependencies

- [Dear ImGui](https://github.com/ocornut/imgui)
- [glfw](https://github.com/GLFW/glfw)
- [vk-bootstrap](https://github.com/charles-lunarg/vk-bootstrap)
- [dyvk](https://github.com/karnkaul/dyvk)
- [ktl](https://github.com/karnkaul/ktl)

[Original repository](https://github.com/karnkaul/dibs)

[LICENCE](LICENSE)
