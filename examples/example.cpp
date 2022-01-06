#include <imgui.h>
#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <iostream>

int main() {
	std::cout << "dibs v" << dibs::version << '\n';
	auto instance = dibs::Instance::Builder{}();
	if (!instance) {
		std::cerr << "fail! error: " << (int)instance.error() << '\n';
		return 0;
	}
	auto closed = instance->signals().onClosed();
	closed += []() { std::cout << "closed\n"; };
	while (!instance->closing()) {
		instance->poll();
		auto frame = dibs::Frame(*instance, 0x663333ff);
		// tick / draw
		static bool s_showDemo = true;
		if (s_showDemo) { ImGui::ShowDemoWindow(&s_showDemo); }
	}
}
