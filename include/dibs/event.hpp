#pragma once
#include <dibs/vec2.hpp>
#include <cassert>

namespace dibs {
namespace detail {
// internal usage
struct EventBuilder;
} // namespace detail

using MouseXY = dvec2;

class Event {
  public:
	enum class Type : std::uint8_t {
		eNone,
		eClosed,
		eFocusChange,
		eMaximize,
		eIconify,
		eWindowResize,
		eFramebufferResize,
		eCursor,
		eScroll,
		eKey,
		eMouseButton,
		eText,
	};

	struct Key {
		int scancode;
		int key;
		int action;
		int mods;
	};

	struct Button {
		int button;
		int action;
		int mods;
	};

	Type type() const noexcept { return m_type; }

	bool closed() const noexcept { return (check(Type::eClosed), true); }
	bool focusGained() const noexcept { return (check(Type::eFocusChange), m_payload.b); }
	bool maximized() const noexcept { return (check(Type::eMaximize), m_payload.b); }
	bool iconified() const noexcept { return (check(Type::eIconify), m_payload.b); }
	uvec2 framebufferSize() const noexcept { return (check(Type::eFramebufferResize), extent()); }
	uvec2 windowSize() const noexcept { return (check(Type::eWindowResize), extent()); }
	MouseXY cursor() const noexcept { return (check(Type::eCursor), mouseXY()); }
	MouseXY scroll() const noexcept { return (check(Type::eScroll), mouseXY()); }
	Key const& key() const noexcept { return (check(Type::eKey), m_payload.key); }
	Button const& mouseButton() const noexcept { return (check(Type::eMouseButton), m_payload.button); }
	std::uint32_t codepoint() const noexcept { return (check(Type::eText), m_payload.u32); }

  private:
	void check([[maybe_unused]] Type const type) const noexcept { assert(m_type == type); }
	uvec2 extent() const noexcept { return {m_payload.extent.x, m_payload.extent.y}; }
	MouseXY mouseXY() const noexcept { return {m_payload.mouseXY.x, m_payload.mouseXY.y}; }

	template <typename T>
	struct v2 {
		T x;
		T y;
	};

	union {
		Key key;
		Button button;
		v2<std::uint32_t> extent;
		v2<double> mouseXY;
		std::uint32_t u32;
		bool b;
	} m_payload;
	Type m_type = Type::eNone;

	friend struct detail::EventBuilder;
};
} // namespace dibs
