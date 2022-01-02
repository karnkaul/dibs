#include <VkBootstrap.h>
#include <detail/vk_instance.hpp>

namespace dibs::detail {
Result<VKInstance> VKInstance::make(MakeSurface makeSurface, Flags flags) {
	if (!makeSurface) { return Error::eInvalidArg; }
	vk::DynamicLoader dl;
	VULKAN_HPP_DEFAULT_DISPATCHER.init(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));
	vkb::InstanceBuilder vib;
	if (flags.test(Flag::eValidation)) { vib.request_validation_layers(); }
	auto vi = vib.set_app_name("dibs").use_default_debug_messenger().build();
	if (!vi) { return Error::eVulkanInitFailure; }
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vi->instance);
	VKInstance ret;
	ret.instance = vk::UniqueInstance(vi->instance, {nullptr});
	ret.messenger = vk::UniqueDebugUtilsMessengerEXT(vi->debug_messenger, {vi->instance});
	auto surface = makeSurface(vk::Instance(vi->instance));
	if (!surface) { return Error::eVulkanInitFailure; }
	ret.surface = vk::UniqueSurfaceKHR(surface, {vi->instance});
	vkb::PhysicalDeviceSelector vpds(vi.value());
	auto vpd = vpds.require_present().prefer_gpu_device_type(vkb::PreferredDeviceType::discrete).set_surface(surface).select();
	if (!vpd) { return Error::eVulkanInitFailure; }
	ret.gpu = {vk::PhysicalDeviceProperties(vpd->properties), vk::PhysicalDevice(vpd->physical_device)};
	vkb::DeviceBuilder vdb(vpd.value());
	auto vd = vdb.build();
	if (!vd) { return Error::eVulkanInitFailure; }
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vd->device);
	ret.device = vk::UniqueDevice(vd->device, {nullptr});
	auto queue = vd->get_queue(vkb::QueueType::graphics);
	auto qfam = vd->get_queue_index(vkb::QueueType::graphics);
	if (!queue || !qfam) { return Error::eVulkanInitFailure; }
	ret.queue = VKQueue{vk::Queue(queue.value()), qfam.value()};
	return ret;
}
} // namespace dibs::detail
