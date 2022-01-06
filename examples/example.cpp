#include <imgui.h>				 // Dear ImGui
#include <dibs/dibs.hpp>		 // primary header
#include <dibs/dibs_version.hpp> // version
#include <iostream>

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
	// obtain a signal for when the instance / window will be closed
	auto closed = instance->signals().onClosed();
	// attach a callback (auto detached on signal destruction)
	closed += []() { std::cout << "closed\n"; };
	// start main loop
	while (!instance->closing()) {
		// poll events (and obtain delta time if required)
		[[maybe_unused]] auto const dt = instance->poll();
		// start a new frame with the given clear colour
		auto frame = dibs::Frame(*instance, 0x663333ff);
		// tick / draw
		static bool s_showDemo = true;
		if (s_showDemo) { ImGui::ShowDemoWindow(&s_showDemo); }
	}
}
