#pragma once
#include <dibs/vec2.hpp>
#include <dibs/vk_types.hpp>
#include <ktl/fixed_vector.hpp>
#include <optional>

namespace dibs::detail {
struct VKImage {
	vk::Image image;
	vk::ImageView view;
	vk::Extent2D extent{};
};

struct VKSwapchain {
	ktl::fixed_vector<VKImage, 8> images;
	ktl::fixed_vector<vk::UniqueImageView, 8> views;
	vk::UniqueSwapchainKHR swapchain;
};

struct VKSurface {
	struct Sync {
		vk::Semaphore wait;
		vk::Semaphore ssignal;
		vk::Fence fsignal;
	};

	struct Acquire {
		VKImage image;
		std::uint32_t index{};
	};

	vk::SwapchainCreateInfoKHR info;
	VKSwapchain swapchain;
	vk::SurfaceKHR surface;
	class DeferQueue* deferQueue{};

	static vk::SwapchainCreateInfoKHR makeInfo(VKDevice const& device, vk::SurfaceKHR surface, uvec2 framebuffer) noexcept;

	vk::Result refresh(VKDevice const& device, uvec2 framebuffer);
	std::optional<Acquire> acquire(VKDevice const& device, vk::Semaphore signal, uvec2 framebuffer);
	vk::Result submit(VKDevice const& device, vk::CommandBuffer cb, Sync const& sync, uvec2 framebuffer);
	vk::Result present(VKDevice const& device, Acquire const& acquired, vk::Semaphore wait, uvec2 framebuffer);
};
} // namespace dibs::detail
