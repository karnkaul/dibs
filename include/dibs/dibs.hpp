#pragma once
#include <dibs/error.hpp>
#include <dibs/vec2.hpp>
#include <ktl/enum_flags/enum_flags.hpp>
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

  private:
	struct Impl;
	Instance(std::unique_ptr<Impl>&& impl) noexcept;

	std::unique_ptr<Impl> m_impl;
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
