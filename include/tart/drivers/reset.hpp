#pragma once

#include <tart/drivers/gpio.hpp>

#include <stdint.h>
#include <stddef.h>

namespace tart {
	struct reset_controller {
		constexpr reset_controller() = default;

		virtual void reset(uint16_t block) = 0;
	protected:
		~reset_controller() = default;
	};

	inline constexpr uint16_t reset_triggered = (1 << 0);

	struct block_reset {
		reset_controller *controller;
		uint16_t block;
		uint16_t flags;

		void reset() {
			if (!(flags & reset_triggered))
				controller->reset(block);

			flags |= reset_triggered;
		}
	};
} // namespace tart
