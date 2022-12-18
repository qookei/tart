#pragma once

#include <stdint.h>

namespace tart {
	namespace gpio_flags {
		inline constexpr uint8_t pull_up = (1 << 0);	///< Enable a pull-up
		inline constexpr uint8_t pull_down = (1 << 1);	///< Enable a pull-down
		inline constexpr uint8_t output = (1 << 2);	///< Output enabled
		inline constexpr uint8_t input = (1 << 3);	///< Input enabled
	} // namespace gpio_flags

	// TODO(qookie): Add pin interrupt support
	struct gpio_controller {
		constexpr gpio_controller() = default;

		virtual void configure(uint16_t pin, uint8_t fn, uint8_t flags) = 0;
		virtual void set(uint16_t pin, bool value) = 0;
		virtual void toggle(uint16_t pin) = 0;
		virtual bool get(uint16_t pin) = 0;

	protected:
		~gpio_controller() = default;
	};

	struct gpio_pin {
		gpio_controller *controller;
		uint16_t pin;
		uint8_t fn;
		uint8_t flags;

		void configure() const {
			controller->configure(pin, fn, flags);
		}

		void set(bool value) const {
			controller->set(pin, value);
		}

		void toggle() const {
			controller->toggle(pin);
		}

		bool get() const {
			return controller->get(pin);
		}
	};
} // namespace tart
