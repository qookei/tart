#pragma once

#include <tart/drivers/reset.hpp>
#include <tart/drivers/gpio.hpp>
#include <arch/mem_space.hpp>

namespace tart {

// TODO(qookie): This does not allow for configuring the pins
// that are generally used for QSPI.
struct rp2_gpio final : gpio_controller {
	constexpr rp2_gpio(uintptr_t sio_base, uintptr_t pads_base, uintptr_t gpio_base,
			block_reset *io_reset, block_reset *pads_reset)
	: sio_space_{sio_base}, pads_space_{pads_base}, gpio_space_{gpio_base},
		io_reset_{io_reset}, pads_reset_{pads_reset} { }

	~rp2_gpio() = default;

	void init();

	void configure(uint16_t pin, uint8_t fn, uint8_t flags) override;
	void set(uint16_t pin, bool value) override;
	void toggle(uint16_t pin) override;
	bool get(uint16_t pin) override;

private:
	// TODO(qookie): Remove the :: prefix once tart::arch is gone.
	::arch::mem_space sio_space_;
	::arch::mem_space pads_space_;
	::arch::mem_space gpio_space_;

	block_reset *io_reset_;
	block_reset *pads_reset_;
};

} // namespace tart
