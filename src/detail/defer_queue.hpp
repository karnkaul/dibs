#pragma once
#include <deque>
#include <memory>
#include <vector>

namespace dibs::detail {
class DeferQueue {
  public:
	DeferQueue(std::size_t buffer = 3U) : m_lists(buffer) {}

	template <typename T>
	void defer(T t) {
		m_current.push_back(std::make_unique<Wrap<T>>(std::move(t)));
	}

	void next() {
		m_lists.pop_front();
		m_lists.push_back(std::move(m_current));
	}

  private:
	struct Base {
		virtual ~Base() = default;
	};
	template <typename T>
	struct Wrap : Base {
		T t;
		Wrap(T t) noexcept(std::is_nothrow_move_constructible_v<T>) : t(std::move(t)) {}
	};

	using List = std::vector<std::unique_ptr<Base>>;

	List m_current;
	std::deque<List> m_lists;
};
} // namespace dibs::detail
