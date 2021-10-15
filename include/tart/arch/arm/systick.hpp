#pragma once

#include <arch/mem_space.hpp>

namespace tart::arch {
	inline constexpr ::arch::mem_space systick_space{0xe000e010};

	void systick_init(uint32_t freq);
} // namespace tart::arch
