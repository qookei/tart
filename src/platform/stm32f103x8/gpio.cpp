#include <periph/gpio.hpp>

#include <arch/arm/mem_space.hpp>
#include "gpio_reg.hpp"
#include "rcc.hpp"

namespace gpio {

static bool rcc_gpio_en[5];

static constexpr arch::mem_space gpio_space[5] = {
	0x40010800,
	0x40010C00,
	0x40011000,
	0x40011400,
	0x40011800
};

void setup(pin p, mode d, config c, bool alt_fn) {
	using namespace platform;

	int pidx = p.p;
	int bidx = static_cast<int>(p.b);
	auto &space = gpio_space[bidx];

	if (!rcc_gpio_en[bidx]) {
		rcc::set_periph_enable(
			static_cast<rcc::periph>(
				static_cast<int>(
					rcc::periph::gpio_a) + bidx), true);
		rcc_gpio_en[bidx] = true;
	}

	uint8_t c_ = static_cast<uint8_t>(c);
	uint8_t m_ = static_cast<uint8_t>(d);

	if (d != mode::input && alt_fn)
		c_ += 2;

	if (pidx < 8)
		space.store(reg::cr_low, space.load(reg::cr_low)
				/ cr::pin_mode[pidx](m_)
				/ cr::pin_conf[pidx](c_));
	else
		space.store(reg::cr_high, space.load(reg::cr_high)
				/ cr::pin_mode[pidx - 8](m_)
				/ cr::pin_conf[pidx - 8](c_));
}

void set(pin p, bool b) {
	int pidx = p.p;
	int bidx = static_cast<int>(p.b);
	auto &space = gpio_space[bidx];

	space.store(reg::bit_set_reset, 1 << (b ? pidx : pidx + 16));
}

bool get(pin p) {
	int pidx = p.p;
	int bidx = static_cast<int>(p.b);
	auto &space = gpio_space[bidx];

	return space.load(reg::input_data) & (1 << pidx);
}

} // namespace gpio
