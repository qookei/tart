#pragma once

#include <stdint.h>
#include <atomic>

namespace tart {
	// XXX: This likely should be atomic, but apparently we can't use
	// 64-bit values, _or_ fetch_add.
	inline volatile uint64_t current_time_ = 0;

	inline uint64_t ms_now() {
		return current_time_;
	}
} // namespace tart
