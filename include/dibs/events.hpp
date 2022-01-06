#pragma once
#include <dibs/vec2.hpp>
#include <ktl/delegate.hpp>

namespace dibs {
struct KeyEvent {
	int key{};
	int scancode{};
	int action{};
	int mods{};
};

struct MouseEvent {
	int button{};
	int action{};
	int mods{};
};

using OnFocus = ktl::delegate<bool>;
using OnResize = ktl::delegate<uvec2>;
using OnClosed = ktl::delegate<>;
using OnKeyEvent = ktl::delegate<KeyEvent>;
using OnText = ktl::delegate<std::uint32_t>;
using OnCursor = ktl::delegate<tvec2<double>>;
using OnMouseButton = ktl::delegate<MouseEvent>;
using OnScroll = OnCursor;
using OnMaximize = OnFocus;
using OnIconify = OnFocus;
} // namespace dibs
