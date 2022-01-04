#include <detail/expect.hpp>
#include <detail/log.hpp>
#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <instance_impl.hpp>

namespace dibs {
namespace {
Result<Glfw> makeGlfw(char const* title, uvec2 const extent, Instance::Flags const flags) noexcept {
	if (detail::g_window) { return Error::eDuplicateInstance; }
	if (extent.x == 0U || extent.y == 0U) { return Error::eInvalidArg; }
	auto instance = detail::GlfwInstance::make();
	if (!instance) { return Error::eGlfwInitFailure; }
	if (!glfwVulkanSupported()) { return Error::eUnsupportedPlatform; }
	auto window = instance->makeWindow(title, extent, flags);
	if (!window) { return Error::eWindowCreationFailure; }
	return Glfw{std::move(instance), std::move(window)};
}

bool centre(GLFWwindow* const window) {
	auto const monitor = glfwGetPrimaryMonitor();
	if (!monitor) { return false; }
	auto const mode = glfwGetVideoMode(monitor);
	if (!mode) { return false; }
	int w{}, h{};
	glfwGetWindowSize(window, &w, &h);
	if (w <= 0 || h <= 0) { return false; }
	glfwSetWindowPos(window, (mode->width - w) / 2, (mode->height - h) / 2);
	return true;
}

uvec2 getFramebufferSize(GLFWwindow* const window) noexcept {
	int w{}, h{};
	glfwGetFramebufferSize(window, &w, &h);
	return {std::uint32_t(w), std::uint32_t(h)};
}

uvec2 getWindowSize(GLFWwindow* const window) noexcept {
	int w{}, h{};
	glfwGetWindowSize(window, &w, &h);
	return {std::uint32_t(w), std::uint32_t(h)};
}

VKDevice initDevice(detail::VKInstance const& inst) noexcept {
	VKDevice ret;
	ret.instance = *inst.instance;
	ret.device = *inst.device;
	ret.gpu = inst.gpu;
	ret.queue = inst.queue;
	return ret;
}

FrameSync initFrameSync(vk::Device const device, std::uint32_t const queueFamily) {
	using CPCFB = vk::CommandPoolCreateFlagBits;
	static constexpr vk::CommandPoolCreateFlags pool_flags_v = CPCFB::eTransient | CPCFB::eResetCommandBuffer;
	static constexpr vk::CommandBufferLevel cb_lvl_v = vk::CommandBufferLevel::ePrimary;
	FrameSync ret;
	for (std::size_t i = 0; i < FrameSync::frames_v; ++i) {
		ret.sync[i].draw = device.createSemaphoreUnique({});
		ret.sync[i].present = device.createSemaphoreUnique({});
		ret.sync[i].drawn = device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
		ret.sync[i].pool = device.createCommandPoolUnique(vk::CommandPoolCreateInfo(pool_flags_v, queueFamily));
		ret.sync[i].cb = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(*ret.sync[i].pool, cb_lvl_v, 1U)).front();
	}
	return ret;
}

vk::UniqueRenderPass makeRenderPass(vk::Device device, vk::Format colour, bool autoTransition) {
	vk::AttachmentDescription attachment;
	attachment.format = colour;
	attachment.samples = vk::SampleCountFlagBits::e1;
	attachment.loadOp = vk::AttachmentLoadOp::eClear;
	attachment.storeOp = vk::AttachmentStoreOp::eStore;
	attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	if (autoTransition) {
		attachment.initialLayout = vk::ImageLayout::eUndefined;
		attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
	} else {
		attachment.initialLayout = attachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
	}
	vk::AttachmentReference color_attachment;
	color_attachment.attachment = 0;
	color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
	vk::SubpassDescription subpass;
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment;
	vk::SubpassDependency dependency;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
	vk::RenderPassCreateInfo info;
	info.attachmentCount = 1;
	info.pAttachments = &attachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = 1;
	info.pDependencies = &dependency;
	return device.createRenderPassUnique(info);
}

vk::UniqueFramebuffer makeFramebuffer(vk::Device device, vk::RenderPass pass, detail::VKImage const& target) {
	EXPECT(target.extent.width > 0U && target.extent.height > 0U);
	return device.createFramebufferUnique(vk::FramebufferCreateInfo({}, pass, 1U, &target.view, target.extent.width, target.extent.height, 1U));
}

template <typename T, typename U = T>
using TPair = std::pair<T, U>;

struct ImageBarrier {
	TPair<vk::AccessFlags> access;
	TPair<vk::PipelineStageFlags> stages;
	vk::CommandBuffer cb;
	vk::Image image;

	void operator()(TPair<vk::ImageLayout> const& layouts) const {
		vk::ImageMemoryBarrier barrier;
		barrier.oldLayout = layouts.first;
		barrier.newLayout = layouts.second;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.levelCount = 1U;
		barrier.subresourceRange.layerCount = 1U;
		barrier.srcAccessMask = access.first;
		barrier.dstAccessMask = access.second;
		cb.pipelineBarrier(stages.first, stages.second, {}, {}, {}, barrier);
	}
};
} // namespace

Instance::Instance(std::unique_ptr<Impl>&& impl) noexcept : m_impl(std::move(impl)) {}
Instance::Instance(Instance&&) noexcept = default;
Instance& Instance::operator=(Instance&&) noexcept = default;
Instance::~Instance() noexcept {
	if (m_impl) {
		m_impl->device.device.waitIdle();
		detail::g_window = {};
	}
}

bool Instance::closing() const noexcept {
	EXPECT(m_impl);
	return glfwWindowShouldClose(m_impl->glfw.window);
}

Time Instance::poll() noexcept {
	EXPECT(m_impl);
	glfwPollEvents();
	auto const t = Clock::now();
	return t - std::exchange(m_impl->elapsed, t);
}

uvec2 Instance::framebufferSize() const noexcept { return getFramebufferSize(m_impl->glfw.window); }
uvec2 Instance::windowSize() const noexcept { return getWindowSize(m_impl->glfw.window); }

Frame::Frame(Instance const& instance, Clear const& clear) : m_clear(clear), m_instance(instance) {
	EXPECT(m_instance.m_impl && !m_instance.m_impl->acquired); // must not have already acquired an image
	auto impl = m_instance.m_impl.get();
	auto& sync = impl->frameSync.get();
	// acquire next swapchain image to render to
	impl->acquired = impl->surface.acquire(impl->device, *sync.draw, m_instance.framebufferSize());
}

Frame::~Frame() {
	auto impl = m_instance.m_impl.get();
	if (impl->acquired) {
		static constexpr auto max_wait_v = std::numeric_limits<std::uint64_t>::max();
		auto& sync = impl->frameSync.get();
		// wait for previous draw on this image to complete
		impl->device.device.waitForFences(*sync.drawn, true, max_wait_v);
		impl->device.device.resetFences(*sync.drawn);
		// start recording
		sync.cb.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
		// transition image for shading
		ImageBarrier ib;
		ib.image = impl->acquired->image.image;
		ib.cb = sync.cb;
		ib.access = {{}, vk::AccessFlagBits::eColorAttachmentWrite};
		ib.stages = {vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput};
		ib({vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal});
		// make framebuffer corresponding to current image
		sync.framebuffer = makeFramebuffer(impl->device.device, *impl->renderPass, impl->acquired->image);
		// perform render pass
		vk::ClearValue cv = vk::ClearColorValue(m_clear);
		vk::RenderPassBeginInfo rpbi;
		rpbi.renderPass = *impl->renderPass;
		rpbi.framebuffer = *sync.framebuffer;
		rpbi.renderArea.extent = impl->acquired->image.extent;
		rpbi.clearValueCount = 1U;
		rpbi.pClearValues = &cv;
		sync.cb.beginRenderPass(rpbi, vk::SubpassContents::eInline);
		// TODO: render Dear ImGui draw data
		sync.cb.endRenderPass();
		// transition image for presentation
		ib.access = {vk::AccessFlagBits::eColorAttachmentWrite, {}};
		ib.stages = {vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe};
		ib({vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR});
		// stop recording
		sync.cb.end();
		uvec2 const fb = m_instance.framebufferSize();
		// submit commands and present image
		auto res = impl->surface.submit(impl->device, sync.cb, {*sync.draw, *sync.present, *sync.drawn}, fb);
		if (res == vk::Result::eSuccess) { res = impl->surface.present(impl->device, *impl->acquired, *sync.present, fb); }
		// swap buffers
		impl->frameSync.next();
		// reset acquired image (submitted to presentation engine)
		impl->acquired.reset();
		EXPECT(res == vk::Result::eSuccess || res == vk::Result::eSuboptimalKHR || res == vk::Result::eErrorOutOfDateKHR);
	}
}

bool Frame::ready() const noexcept { return m_instance.m_impl->acquired.has_value(); }

uvec2 Frame::extent() const noexcept {
	auto const ret = m_instance.m_impl->surface.info.imageExtent;
	return {ret.width, ret.height};
}

Result<Instance> Instance::Builder::operator()() const {
	auto glfw = makeGlfw(m_title.data(), m_extent, m_flags);
	if (!glfw) { return glfw.error(); }
	auto makeSurface = [&glfw](vk::Instance inst) {
		VkSurfaceKHR ret;
		glfwCreateWindowSurface(inst, glfw->window, nullptr, &ret);
		return vk::SurfaceKHR(ret);
	};
	auto vulkan = detail::VKInstance::make(std::move(makeSurface), detail::VKInstance::Flag::eValidation);
	if (!vulkan) { return vulkan.error(); }
	if (!centre(glfw->window)) { log("Failed to centre window"); }
	auto const vkd = initDevice(*vulkan);
	detail::VKSurface surface;
	surface.surface = *vulkan->surface;
	if (surface.refresh(vkd, getFramebufferSize(glfw->window)) != vk::Result::eSuccess) { return Error::eVulkanInitFailure; }
	// all checks passed
	log("Using GPU: {}", vulkan->gpu.properties.deviceName);
	auto impl = std::make_unique<Instance::Impl>();
	impl->glfw = std::move(glfw).value();
	impl->vulkan = std::move(vulkan).value();
	impl->device = vkd;
	impl->surface = std::move(surface);
	impl->frameSync = initFrameSync(vkd.device, vkd.queue.family);
	impl->renderPass = makeRenderPass(vkd.device, impl->surface.info.imageFormat, false);
	if (!m_flags.test(Flag::eHidden)) { glfwShowWindow(impl->glfw.window); }
	return Instance(std::move(impl));
}
} // namespace dibs
