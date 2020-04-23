#include "gpio.hpp"
#include "gpio_reg.hpp"

#include "rcc.hpp"

namespace platform::gpio {

static bool rcc_gpio_en[5];

void setup_pin(bank b, int pin, mode m, conf c) {
	int bidx = static_cast<int>(b);
	auto &space = gpio_space[bidx];

	if (!rcc_gpio_en[bidx]) {
		rcc::set_periph_enable(
			static_cast<rcc::periph>(
				static_cast<int>(
					rcc::periph::gpio_a) + bidx), true);
		rcc_gpio_en[bidx] = true;
	}

	if (pin < 8)
		space.store(reg::cr_low, space.load(reg::cr_low)
				/ cr::pin_mode[pin](m)
				/ cr::pin_conf[pin](c));
	else
		space.store(reg::cr_high, space.load(reg::cr_high)
				/ cr::pin_mode[pin - 8](m)
				/ cr::pin_conf[pin - 8](c));

}

void set_pin(bank b, int pin, bool high) {
	int bidx = static_cast<int>(b);
	auto &space = gpio_space[bidx];

	space.store(reg::bit_set_reset, 1 << (high ? pin : pin + 16));
}

bool get_pin(bank b, int pin) {
	int bidx = static_cast<int>(b);
	auto &space = gpio_space[bidx];

	return space.load(reg::input_data) & (1 << pin);
}

} // namespace platform::gpio
