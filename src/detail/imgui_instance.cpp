#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <detail/imgui_instance.hpp>
#include <limits>

namespace dibs::detail {
namespace {
vk::UniqueDescriptorPool makePool(vk::Device device, std::uint32_t count) {
	vk::DescriptorPoolSize pool_sizes[] = {
		{vk::DescriptorType::eSampler, count},
		{vk::DescriptorType::eCombinedImageSampler, count},
		{vk::DescriptorType::eSampledImage, count},
		{vk::DescriptorType::eStorageImage, count},
		{vk::DescriptorType::eUniformTexelBuffer, count},
		{vk::DescriptorType::eStorageTexelBuffer, count},
		{vk::DescriptorType::eUniformBuffer, count},
		{vk::DescriptorType::eStorageBuffer, count},
		{vk::DescriptorType::eUniformBufferDynamic, count},
		{vk::DescriptorType::eStorageBufferDynamic, count},
		{vk::DescriptorType::eInputAttachment, count},
	};
	vk::DescriptorPoolCreateInfo dpci;
	dpci.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	dpci.poolSizeCount = sizeof(pool_sizes) / sizeof(vk::DescriptorPoolSize);
	dpci.maxSets = count * dpci.poolSizeCount;
	dpci.pPoolSizes = pool_sizes;
	return device.createDescriptorPoolUnique(dpci);
}
} // namespace

Unique<ImGuiInstance, ImGuiInstance::Deleter> ImGuiInstance::make(VKDevice const& device, Info const& info) {
	static vk::Instance s_inst;
	static vk::DynamicLoader s_dl;
	s_inst = device.instance;
	auto const fn = [](char const* f, void*) { return s_dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr")(s_inst, f); };
	ImGui_ImplVulkan_LoadFunctions(fn);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(info.window, true);
	ImGui_ImplVulkan_InitInfo initInfo = {};
	Unique<ImGuiInstance, Deleter> ret;
	ret.get().pool = makePool(device.device, 1000U);
	initInfo.Instance = device.instance;
	initInfo.Device = device.device;
	initInfo.PhysicalDevice = device.gpu.device;
	initInfo.Queue = device.queue.queue;
	initInfo.QueueFamily = device.queue.family;
	initInfo.MinImageCount = info.minImageCount;
	initInfo.ImageCount = info.imageCount;
	initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	initInfo.DescriptorPool = static_cast<VkDescriptorPool>(*ret->pool);
	if (!ImGui_ImplVulkan_Init(&initInfo, info.renderPass)) { return {}; }
	vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device.queue.family);
	auto cpool = device.device.createCommandPoolUnique(poolInfo);
	vk::CommandBufferAllocateInfo commandBufferInfo(*cpool, vk::CommandBufferLevel::ePrimary, 1U);
	auto commandBuffer = device.device.allocateCommandBuffers(commandBufferInfo).front();
	commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
	commandBuffer.end();
	vk::SubmitInfo endInfo;
	endInfo.commandBufferCount = 1U;
	endInfo.pCommandBuffers = &commandBuffer;
	auto done = device.device.createFenceUnique({});
	device.queue.queue.submit(endInfo, *done);
	device.device.waitForFences(*done, true, std::numeric_limits<std::uint64_t>::max());
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	return ret;
}

void ImGuiInstance::Deleter::operator()(ImGuiInstance const&) const {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiInstance::newFrame() const {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiInstance::endFrame() const { ImGui::Render(); }

void ImGuiInstance::render(vk::CommandBuffer cb) const { ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb); }
} // namespace dibs::detail
