#pragma once

#include <stdint.h>
#include <frg/tuple.hpp>

namespace tart::chip {
	frg::tuple<uintptr_t, uintptr_t> get_heap_area();
} // namespace tart::chip
