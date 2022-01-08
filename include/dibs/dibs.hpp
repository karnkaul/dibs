#pragma once
#include <dibs/error.hpp>
#include <dibs/event.hpp>
#include <dibs/rgba.hpp>
#include <ktl/enum_flags/enum_flags.hpp>
#include <chrono>
#include <memory>
#include <span>

namespace dibs {
struct Poll {
	std::span<Event const> events;
	std::chrono::duration<float> dt{};
};

class Instance {
  public:
	enum class Flag { eBorderless, eNoResize, eHidden, eMaximized };
	using Flags = ktl::enum_flags<Flag, std::uint8_t>;

	class Builder;

	Instance(Instance&&) noexcept;
	Instance& operator=(Instance&&) noexcept;
	~Instance() noexcept;

	bool closing() const noexcept;
	Poll poll() noexcept;

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
	[[nodiscard]] Frame(Instance const& instance, RGBA clear = {0x22, 0x22, 0x22});
	~Frame();

	bool ready() const noexcept;
	uvec2 extent() const noexcept;

  private:
	RGBA m_clear;
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
