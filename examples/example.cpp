#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>
#include <iostream>

int main() {
	std::cout << "dibs v" << dibs::version << '\n';
	auto instance = dibs::Instance();
}
