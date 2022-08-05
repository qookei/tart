#pragma once

#include <tart/drivers/reset.hpp>
#include <arch/mem_space.hpp>

namespace tart {
	struct rp2_resets final : reset_controller {
		constexpr rp2_resets(uintptr_t base)
		: space_{base} {}

		void reset(uint16_t block) override;

	private:
		::arch::mem_space space_;
	};
} // namespace tart
