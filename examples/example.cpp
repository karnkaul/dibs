#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <iostream>

int main() {
	std::cout << "dibs v" << dibs::version << '\n';
	auto instance = dibs::Instance::Builder{}();
	if (!instance) {
		std::cerr << "fail! error: " << (int)instance.error() << "\n";
		return 0;
	}
	while (!instance->closing()) {
		instance->poll();
		auto frame = dibs::Frame(*instance, {0.4f, 0.2f, 0.2f, 1.0f});
		// tick / draw
	}
}
