#include <tart/drivers/gpio/rp2-gpio.hpp>

namespace {
	constexpr auto ctrl_reg(uint16_t pin) {
		return arch::bit_register<uint32_t>{pin * 8 + 0x04};
	}

	constexpr auto pads_reg(uint16_t pin) {
		return arch::bit_register<uint32_t>{pin * 4 + 0x04};
	}

	namespace pads {
		inline constexpr arch::field<uint32_t, bool> slew_fast{0, 1};
		inline constexpr arch::field<uint32_t, bool> schmitt{1, 1};
		inline constexpr arch::field<uint32_t, bool> pull_down{2, 1};
		inline constexpr arch::field<uint32_t, bool> pull_up{3, 1};
		inline constexpr arch::field<uint32_t, uint8_t> drive{4, 2};
		inline constexpr arch::field<uint32_t, bool> input_en{6, 1};
		inline constexpr arch::field<uint32_t, bool> output_dis{7, 1};
	} // namespace pads

	namespace ctrl {
		inline constexpr arch::field<uint32_t, uint8_t> funcsel{0, 4};
		inline constexpr arch::field<uint32_t, uint8_t> outover{8, 2};
		inline constexpr arch::field<uint32_t, uint8_t> oeover{12, 2};
		inline constexpr arch::field<uint32_t, uint8_t> inover{16, 2};
		inline constexpr arch::field<uint32_t, uint8_t> irqover{28, 2};
	} // namespace ctrl

	namespace sio_reg {
		inline constexpr arch::scalar_register<uint32_t> gpio_in{0x04};
		inline constexpr arch::scalar_register<uint32_t> gpio_out_set{0x14};
		inline constexpr arch::scalar_register<uint32_t> gpio_out_clr{0x18};
		inline constexpr arch::scalar_register<uint32_t> gpio_out_xor{0x1C};
		inline constexpr arch::scalar_register<uint32_t> gpio_out_en_set{0x24};
		inline constexpr arch::scalar_register<uint32_t> gpio_out_en_clr{0x28};
	} // namespace sio_reg
} // namespace anonymous

namespace tart {

void rp2_gpio::init() {
	if (io_reset_)
		io_reset_->reset();
	if (pads_reset_)
		pads_reset_->reset();
}

void rp2_gpio::configure(uint16_t pin, uint8_t fn, uint8_t flags) {
	gpio_space_.store(ctrl_reg(pin),
			ctrl::funcsel(fn)
			| ctrl::outover(0)
			| ctrl::oeover(0)
			| ctrl::inover(0)
			| ctrl::irqover(0));

	pads_space_.store(pads_reg(pin),
			pads::slew_fast(false)
			| pads::schmitt(true)
			| pads::pull_down(flags & gpio_flags::pull_down)
			| pads::pull_up(flags & gpio_flags::pull_down)
			| pads::drive(2) // 4mA nominal (default)
			| pads::input_en(flags & gpio_flags::input)
			| pads::output_dis(!(flags & gpio_flags::output)));

	if (fn == 5) {
		sio_space_.store(
			(flags & gpio_flags::output)
				? sio_reg::gpio_out_en_set
				: sio_reg::gpio_out_en_clr,
			(1 << pin));
	}
}

void rp2_gpio::set(uint16_t pin, bool value) {
	sio_space_.store(
		value
			? sio_reg::gpio_out_set
			: sio_reg::gpio_out_clr,
		(1 << pin));
}

void rp2_gpio::toggle(uint16_t pin) {
	sio_space_.store(
		sio_reg::gpio_out_xor,
		(1 << pin));
}

bool rp2_gpio::get(uint16_t pin) {
	return sio_space_.load(sio_reg::gpio_in) & (1 << pin);
}

} // namespace tart
