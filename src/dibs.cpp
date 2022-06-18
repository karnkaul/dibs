#include <detail/expect.hpp>
#include <detail/log.hpp>
#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <instance_impl.hpp>

namespace dibs {
namespace {
Result<Glfw> makeGlfw(char const* title, uvec2 const extent, Instance::Flags const flags) noexcept {
	if (detail::g_glfwData.window) { return Error::eDuplicateInstance; }
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
	ivec2 const msize{mode->width, mode->height};
	ivec2 wsize;
	glfwGetWindowSize(window, &wsize.x, &wsize.y);
	if (wsize.x <= 0 || wsize.y <= 0) { return false; }
	auto const centre = (msize - wsize) / ivec2{2, 2};
	glfwSetWindowPos(window, centre.x, centre.y);
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

std::span<std::string const> Event::fileDrop() const noexcept {
	if (m_type == Type::eFileDrop && detail::g_glfwData.eventStorage && m_payload.index < detail::g_glfwData.eventStorage->drops.size()) {
		return detail::g_glfwData.eventStorage->drops[m_payload.index];
	}
	return {};
}

Instance::Instance(std::unique_ptr<Impl>&& impl) noexcept : m_impl(std::move(impl)) {}
Instance::Instance(Instance&&) noexcept = default;
Instance& Instance::operator=(Instance&&) noexcept = default;
Instance::~Instance() noexcept {
	if (m_impl) {
		m_impl->device.device.waitIdle();
		detail::g_glfwData = {};
	}
}

bool Instance::closing() const noexcept {
	EXPECT(m_impl);
	return glfwWindowShouldClose(m_impl->glfw.window);
}

Poll Instance::poll() noexcept {
	EXPECT(m_impl);
	m_impl->events.clear();
	m_impl->eventStorage = {};
	glfwPollEvents();
	auto const t = Clock::now();
	Poll ret;
	ret.dt = t - std::exchange(m_impl->elapsed, t);
	ret.events = m_impl->events;
	return ret;
}

uvec2 Instance::framebufferSize() const noexcept { return getFramebufferSize(m_impl->glfw.window); }
uvec2 Instance::windowSize() const noexcept { return getWindowSize(m_impl->glfw.window); }
std::string_view Instance::clipboard() const noexcept {
	auto const ret = glfwGetClipboardString(nullptr);
	return ret ? ret : std::string_view();
}
void Instance::clipboard(std::string_view text) noexcept { glfwSetClipboardString(nullptr, text.data()); }
void Instance::sizeLimits(std::optional<uvec2> min, std::optional<uvec2> max) noexcept {
	int const minX = min ? int(min->x) : GLFW_DONT_CARE;
	int const maxX = max ? int(max->x) : GLFW_DONT_CARE;
	int const minY = min ? int(min->y) : GLFW_DONT_CARE;
	int const maxY = max ? int(max->y) : GLFW_DONT_CARE;
	glfwSetWindowSizeLimits(m_impl->glfw.window, minX, minY, maxX, maxY);
}

void Instance::aspectRatio(float ratio) noexcept { glfwSetWindowAspectRatio(m_impl->glfw.window, int(ratio * 1000.0f), 1000); }
void Instance::title(std::string_view utf8) noexcept { glfwSetWindowTitle(m_impl->glfw.window, utf8.data()); }

void Instance::icon(std::span<const Bitmap> bitmaps) noexcept {
	std::vector<GLFWimage> images;
	images.reserve(bitmaps.size());
	for (auto const bitmap : bitmaps) {
		images.push_back(GLFWimage{int(bitmap.extent.x), int(bitmap.extent.y), const_cast<unsigned char*>(bitmap.bytes.data())});
	}
	glfwSetWindowIcon(m_impl->glfw.window, int(images.size()), images.data());
}

Frame::Frame(Instance const& instance, RGBA const clear) : m_clear(clear), m_instance(instance) {
	EXPECT(m_instance.m_impl && !m_instance.m_impl->acquired); // must not have already acquired an image
	auto impl = m_instance.m_impl.get();
	auto& sync = impl->frameSync.get();
	// acquire next swapchain image to render to
	impl->acquired = impl->surface.acquire(impl->device, *sync.draw, m_instance.framebufferSize());
	impl->imgui->newFrame();
}

Frame::~Frame() {
	auto impl = m_instance.m_impl.get();
	impl->imgui->endFrame();
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
		m_clear.a = 0xff;
		vk::ClearValue cv = vk::ClearColorValue(m_clear.array());
		vk::RenderPassBeginInfo rpbi;
		rpbi.renderPass = *impl->renderPass;
		rpbi.framebuffer = *sync.framebuffer;
		rpbi.renderArea.extent = impl->acquired->image.extent;
		rpbi.clearValueCount = 1U;
		rpbi.pClearValues = &cv;
		sync.cb.beginRenderPass(rpbi, vk::SubpassContents::eInline);
		impl->imgui->render(sync.cb);
		sync.cb.endRenderPass();
		// transition image for presentation
		ib.access = {vk::AccessFlagBits::eColorAttachmentWrite, {}};
		ib.stages = {vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe};
		ib({vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR});
		// stop recording
		sync.cb.end();
		// submit commands and present image
		auto const res = impl->surface.submit(impl->device, sync.cb, {*sync.draw, *sync.present, *sync.drawn});
		EXPECT(res == vk::Result::eSuccess);
		if (res != vk::Result::eSuccess) { return; }
		auto const pres = impl->surface.present(impl->device, *impl->acquired, *sync.present, m_instance.framebufferSize());
		EXPECT(pres);
		// swap buffers
		impl->frameSync.next();
		impl->deferQueue.next();
		// reset acquired image (submitted to presentation engine)
		impl->acquired.reset();
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
	auto renderPass = makeRenderPass(vkd.device, surface.info.imageFormat, false);
	auto imgui = detail::ImGuiInstance::make(vkd, {glfw->window, *renderPass, 2U, surface.info.minImageCount});
	if (!imgui) { return Error::ImGuiInitFailure; }
	// all checks passed
	log("Using GPU: {}", std::string(vulkan->gpu.properties.deviceName.begin(), vulkan->gpu.properties.deviceName.end()));
	auto impl = std::make_unique<Instance::Impl>();
	impl->glfw = std::move(glfw).value();
	impl->vulkan = std::move(vulkan).value();
	impl->device = vkd;
	impl->surface = std::move(surface);
	impl->surface.deferQueue = &impl->deferQueue;
	impl->frameSync = initFrameSync(vkd.device, vkd.queue.family);
	impl->renderPass = std::move(renderPass);
	impl->imgui = std::move(imgui);
	impl->events.reserve(512U);
	detail::g_glfwData = {impl->glfw.window, &impl->events, &impl->eventStorage};
	if (!m_flags.test(Flag::eHidden)) { glfwShowWindow(impl->glfw.window); }
	return Instance(std::move(impl));
}
} // namespace dibs
