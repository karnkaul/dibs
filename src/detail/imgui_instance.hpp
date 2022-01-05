#pragma once
#include <detail/unique.hpp>
#include <dibs/vk_types.hpp>

struct GLFWwindow;

namespace dibs {
class Instance;
namespace detail {
struct ImGuiInstance {
	struct Info;

	vk::UniqueDescriptorPool pool;

	bool operator==(ImGuiInstance const& rhs) const { return (!pool && !rhs.pool) || *pool == *rhs.pool; };

	struct Deleter {
		void operator()(ImGuiInstance const& di) const;
	};

	static Unique<ImGuiInstance, Deleter> make(VKDevice const& device, Info const& info);

	void newFrame() const;
	void endFrame() const;
	void render(vk::CommandBuffer cb) const;
};

struct ImGuiInstance::Info {
	GLFWwindow* window{};
	vk::RenderPass renderPass;
	std::uint32_t minImageCount{};
	std::uint32_t imageCount{};
};

using UniqueImGui = Unique<ImGuiInstance, ImGuiInstance::Deleter>;
} // namespace detail
} // namespace dibs
