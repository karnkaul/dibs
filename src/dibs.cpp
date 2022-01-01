#include <dibs/dibs.hpp>
#include <dibs/dibs_version.hpp>

namespace dibs {
struct Instance::Impl {};

Instance::Instance() : m_impl(std::make_unique<Impl>()) {}
Instance::Instance(Instance&&) noexcept = default;
Instance& Instance::operator=(Instance&&) noexcept = default;
Instance::~Instance() noexcept = default;
} // namespace dibs
