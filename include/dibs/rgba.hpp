#pragma once
#include <array>
#include <cstdint>
#include <span>

namespace dibs {
struct RGBA {
	std::uint8_t r{};
	std::uint8_t g{};
	std::uint8_t b{};
	std::uint8_t a{};

	static constexpr std::uint8_t cast(float channel) noexcept { return static_cast<std::uint8_t>(channel * 0xff); }
	static constexpr float cast(std::uint8_t channel) noexcept { return static_cast<float>(channel) / 0xff; }
	static constexpr RGBA make(std::span<float, 4> const& rgba) noexcept { return {cast(rgba[0]), cast(rgba[1]), cast(rgba[2]), cast(rgba[3])}; }
	constexpr std::array<float, 4> array() const noexcept { return {cast(r), cast(g), cast(b), cast(a)}; }
};
} // namespace dibs
