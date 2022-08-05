#pragma once

#include <tart/drivers/gpio.hpp>

#include <stdint.h>
#include <stddef.h>

namespace tart {
	struct reset_controller {
		virtual void reset(uint16_t block) = 0;
	protected:
		virtual ~reset_controller() = default;
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
