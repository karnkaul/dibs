#pragma once
#include <detail/defer_queue.hpp>
#include <detail/glfw_instance.hpp>
#include <detail/imgui_instance.hpp>
#include <detail/vk_instance.hpp>
#include <detail/vk_surface.hpp>
#include <dibs/dibs.hpp>

namespace dibs {
using Clock = std::chrono::steady_clock;

struct Glfw {
	detail::UniqueGlfw instance;
	detail::UniqueWindow window;
};

struct FrameSync {
	static constexpr std::size_t frames_v = 2U;

	struct Sync {
		vk::UniqueSemaphore draw;
		vk::UniqueSemaphore present;
		vk::UniqueFence drawn;
		vk::UniqueCommandPool pool;
		vk::CommandBuffer cb;
		vk::UniqueFramebuffer framebuffer;
	};

	Sync sync[frames_v];
	std::size_t index{};

	Sync& get() noexcept { return sync[index]; }
	void next() noexcept { index = (index + 1) % frames_v; }
};

struct Delegates {
	OnFocus focus;
	OnResize windowResize;
	OnResize framebufferResize;
	OnClosed closed;
	OnKeyEvent key;
	OnText text;
	OnCursor cursor;
	OnMouseButton mouseButton;
	OnScroll scroll;
	OnMaximize maximized;
	OnIconify iconified;
};

struct Instance::Impl {
	Glfw glfw;
	detail::VKInstance vulkan;
	VKDevice device;
	detail::VKSurface surface;
	FrameSync frameSync;
	detail::DeferQueue deferQueue;
	vk::UniqueRenderPass renderPass;
	detail::UniqueImGui imgui;
	Delegates delegates;
	std::optional<detail::VKSurface::Acquire> acquired;
	Clock::time_point elapsed = Clock::now();
};
} // namespace dibs
