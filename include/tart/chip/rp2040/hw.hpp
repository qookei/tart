#include "tart/drivers/reset.hpp"
#include <tart/drivers/gpio/rp2-gpio.hpp>
#include <tart/drivers/reset/rp2-resets.hpp>
#include <tart/drivers/uart/pl011.hpp>
#include <frg/manual_box.hpp>

namespace tart {
	inline rp2_gpio gpio{0xd0000000, 0x4001c000, 0x40014000};
	inline constinit gpio_pin uart0_tx_pin{&gpio, 0, 2, gpio_flags::output};
	inline constinit gpio_pin uart0_rx_pin{&gpio, 1, 2, gpio_flags::input | gpio_flags::pull_up};

	inline rp2_resets resets{0x4000c000};
	inline constinit block_reset uart0_reset{&resets, 22, 0};
	inline constinit block_reset uart1_reset{&resets, 23, 0};

	inline pl011_uart uart0{0x40034000, {&uart0_rx_pin, &uart0_tx_pin}, &uart0_reset};
} // namespace tart
