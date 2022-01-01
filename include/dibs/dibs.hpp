#pragma once
#include <memory>

namespace dibs {
class Instance {
  public:
	Instance();
	Instance(Instance&&) noexcept;
	Instance& operator=(Instance&&) noexcept;
	~Instance() noexcept;

  private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};
} // namespace dibs
