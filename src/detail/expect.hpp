#pragma once
#include <detail/log.hpp>
#include <ktl/debug_trap.hpp>

#define EXPECT(predicate)                                                                                                                                      \
	do {                                                                                                                                                       \
		if (!(predicate)) {                                                                                                                                    \
			::dibs::trace("Expect failed: {}", #predicate);                                                                                                    \
			KTL_DEBUG_TRAP();                                                                                                                                  \
		}                                                                                                                                                      \
	} while (false)
