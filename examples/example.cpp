#include <imgui.h>				 // Dear ImGui
#include <dibs/dibs.hpp>		 // primary header
#include <dibs/dibs_version.hpp> // version
#include <ktl/kformat.hpp>
#include <iostream>

namespace {
struct DibsWindow {
	struct {
		dibs::MouseXY cursor{};
		std::chrono::duration<float> elapsed{};
		std::uint32_t frame{};
		std::uint32_t lastFrame{};
		std::uint32_t fps{};
		dibs::RGBA clear = {0x66, 0x33, 0x33};
	} state;
	bool imguiDemo = true;

	void update(dibs::Poll const& poll) noexcept {
		using namespace std::chrono_literals;
		using Type = dibs::Event::Type;
		for (auto const event : poll.events) {
			switch (event.type()) {
			case Type::eClosed: std::cout << "closed\n"; break;
			case Type::eCursor: state.cursor = event.cursor(); break;
			case Type::eFileDrop: {
				std::cout << "Files dropped:\n";
				for (auto const& path : event.fileDrop()) { std::cout << " - " << path << '\n'; }
				break;
			}
			default: break;
			}
		}
		state.elapsed += poll.dt;
		++state.frame;
		if (state.elapsed >= 1s) {
			state.elapsed = {};
			state.fps = state.frame - std::exchange(state.lastFrame, state.frame);
		}
	}

	void draw() {
		static int s_reset = 0;
		ImGui::SetNextWindowSize({300.0f, 200.0f}, ImGuiCond_Once);
		if (ImGui::Begin("dibs")) {
			ImGui::Text("FPS: %u", state.fps);
			ImGui::Text("Cursor: %f, %f", state.cursor.x, state.cursor.y);
			ImGui::Text("Frame: %6u", state.frame);
			ImGui::Separator();
			auto array = state.clear.array();
			if (ImGui::ColorEdit3("Clear", array.data())) { state.clear = dibs::RGBA::make(array); }
			ImGui::NewLine();
			if (ImGui::Button(ktl::kformat("Reset State ({})", s_reset).c_str())) {
				state = {};
				++s_reset;
			}
			ImGui::SameLine();
			std::string_view const text = imguiDemo ? "Hide" : "Show";
			if (ImGui::Button(ktl::kformat("{} ImGui Demo", text.data()).data())) { imguiDemo = !imguiDemo; }
		}
		ImGui::End();
		if (imguiDemo) { ImGui::ShowDemoWindow(&imguiDemo); }
	}
};
} // namespace

int main() {
	std::cout << "dibs v" << dibs::version << '\n';
	auto builder = dibs::Instance::Builder();
	// configure builder...
	// create instance
	auto instance = builder();
	if (!instance) {
		std::cerr << "fail! error: " << (int)instance.error() << '\n';
		return 0; // (this returns 0 for CI to not fail)
	}
	// start main loop
	DibsWindow window;
	while (!instance->closing()) {
		// poll events (and obtain delta time if required)
		window.update(instance->poll());
		// start a new frame with the given clear colour
		auto frame = dibs::Frame(*instance, window.state.clear);
		// draw
		window.draw();
	}
}
