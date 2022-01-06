# Dear ImGui BootStrap (dibs)

A small quick-start library that provides a GLFW / Vulkan backend for Dear ImGui.

[![Build status](https://ci.appveyor.com/api/projects/status/21t2pkbrjlw8ne9o/branch/main?svg=true)](https://ci.appveyor.com/project/karnkaul/dibs/branch/main)

## Features

- Lightweight wrapper with minimal bloat
- Create a GLFW window, Vulkan instance, device, and swapchain in one call
- Start a new frame with a clear colour in one call
- Reuse a single install across multiple CMake projects

## Usage

`dibs` uses CMake as its build system and for installing / packaging releases (3.20+ required).

### Requirements

1. CMake 3.14+ (3.20+ recommended)
1. C++20 compiler
1. Windows / GNU-Linux with X/Wayland

### Integration

#### Build Source

Manifest this repository somewhere in the project tree via git clone / git submodule / CMake FetchContent / etc., say to `ext/dibs`.

```
git submodule add https://github.com/karnkaul/dibs ext/dibs
cd ext/dibs
git checkout (tag / commit / branch)
```

```cmake
include(FetchContent)
FetchContent_Declare(
  dibs
  GIT_REPOSITORY https://github.com/karnkaul/dibs
  # GIT_TAG (pin to a specific tag / commit)
)
FetchContent_MakeAvailable(dibs)
```

Add the project to the build tree and link to `dibs::dibs`.

```cmake
add_subdirectory(ext/dibs)  # not needed with FetchContent
add_executable(foo)
target_link_libraries(foo PRIVATE dibs::dibs)
```

Configure and build via CMake as usual.

```
# copy presets
cp cmake/CMakePresets.json .

cmake -S . --preset=nc-debug
cmake --build --preset=nc-debug
```

#### Shared Install / Pre-built

Install / download the desired build configurations somewhere, say `~/install/dibs`.

```
cmake --install <binary_path> --prefix=~/install/dibs
```

Require `dibs` to be locatable in downstream projects.

```cmake
find_package(dibs REQUIRED)
add_executable(bar)
target_link_libraries(bar PRIVATE dibs::dibs)
```

Add the install path to `CMAKE_PREFIX_PATH` and pass it during configuration of downstream projects.

```
cmake -S . -DCMAKE_PREFIX_PATH=~/install/dibs
```

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
