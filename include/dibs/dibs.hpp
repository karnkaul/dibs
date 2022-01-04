#pragma once
#include <dibs/error.hpp>
#include <dibs/vec2.hpp>
#include <ktl/enum_flags/enum_flags.hpp>
#include <array>
#include <chrono>
#include <memory>

namespace dibs {
using Time = std::chrono::duration<float>;

class Instance {
  public:
	enum class Flag { eBorderless, eNoResize, eHidden, eMaximized };
	using Flags = ktl::enum_flags<Flag, std::uint8_t>;

	class Builder;

	Instance(Instance&&) noexcept;
	Instance& operator=(Instance&&) noexcept;
	~Instance() noexcept;

	bool closing() const noexcept;
	Time poll() noexcept;

	uvec2 framebufferSize() const noexcept;
	uvec2 windowSize() const noexcept;

  private:
	struct Impl;
	Instance(std::unique_ptr<Impl>&& impl) noexcept;
	std::unique_ptr<Impl> m_impl;
	friend class Bridge;
	friend class Frame;
};

class Frame {
  public:
	// TODO: 32-bit colour
	using Clear = std::array<float, 4>;

	[[nodiscard]] Frame(Instance const& instance, Clear const& clear = {});
	~Frame();

	bool ready() const noexcept;
	uvec2 extent() const noexcept;

  private:
	Clear m_clear;
	Instance const& m_instance;
	friend class Bridge;
};

class Instance::Builder {
  public:
	void extent(uvec2 v) noexcept { m_extent = v; }
	void title(std::string s) noexcept { m_title = std::move(s); }
	void flags(Flags f) noexcept { m_flags = f; }

	Result<Instance> operator()() const;

  private:
	std::string m_title{"Untitled"};
	uvec2 m_extent{1280U, 720U};
	Flags m_flags;
};
} // namespace dibs
