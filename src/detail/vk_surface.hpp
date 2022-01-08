#pragma once
#include <dibs/error.hpp>
#include <dibs/vec2.hpp>
#include <ktl/fixed_vector.hpp>
#include <vulkan/vulkan.hpp>
#include <optional>

namespace dibs {
struct VKDevice;
}

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

enum class PresentOutcome { eSuccess, eNotReady };
using PresentResult = ktl::expected<PresentOutcome, vk::Result>;

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

	static constexpr vk::Result refresh_v[] = {vk::Result::eErrorOutOfDateKHR, vk::Result::eSuboptimalKHR};

	vk::SwapchainCreateInfoKHR info;
	VKSwapchain swapchain;
	vk::SurfaceKHR surface;
	class DeferQueue* deferQueue{};

	static vk::SwapchainCreateInfoKHR makeInfo(VKDevice const& device, vk::SurfaceKHR surface, uvec2 framebuffer) noexcept;

	vk::Result refresh(VKDevice const& device, uvec2 framebuffer);
	std::optional<Acquire> acquire(VKDevice const& device, vk::Semaphore signal, uvec2 framebuffer);
	vk::Result submit(VKDevice const& device, vk::CommandBuffer cb, Sync const& sync);
	PresentResult present(VKDevice const& device, Acquire const& acquired, vk::Semaphore wait, uvec2 framebuffer);
};
} // namespace dibs::detail
