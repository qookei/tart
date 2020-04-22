#pragma once

#include <arch/arm/mem_space.hpp>

namespace platform::flash {
	static constexpr arch::mem_space flash_cfg_space{0x40022000};

	void set_wait_states(int ws);
} // namespace platform::flash
