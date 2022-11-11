#include <tart/drivers/reset/rp2-resets.hpp>
#include <tart/drivers/gpio/rp2-gpio.hpp>
#include <tart/drivers/uart/pl011.hpp>
#include <tart/drivers/clk/rp2-osc.hpp>
#include <tart/drivers/clk/rp2-pll.hpp>
#include <tart/drivers/clk/rp2-clk.hpp>

namespace tart {
	inline rp2_gpio gpio{0xd0000000, 0x4001c000, 0x40014000};
	inline constinit gpio_pin uart0_tx_pin{&gpio, 0, 2, gpio_flags::output};
	inline constinit gpio_pin uart0_rx_pin{&gpio, 1, 2, gpio_flags::input | gpio_flags::pull_up};

	inline rp2_resets resets{0x4000c000};
	inline constinit block_reset sys_pll_reset{&resets, 12, 0};
	inline constinit block_reset usb_pll_reset{&resets, 13, 0};
	inline constinit block_reset uart0_reset{&resets, 22, 0};
	inline constinit block_reset uart1_reset{&resets, 23, 0};

	inline pl011_uart uart0{0x40034000, {&uart0_rx_pin, &uart0_tx_pin}, &uart0_reset};

	inline rp2_xosc xosc{0x40024000, 12_MHz};

	inline constexpr rp2_pll_params sys_pll_params =
		rp2_pll_params::compute(12_MHz, 125_MHz);

	inline constexpr rp2_pll_params usb_pll_params =
		rp2_pll_params::compute(12_MHz, 48_MHz);

	inline rp2_pll sys_pll{0x40028000, sys_pll_params, &sys_pll_reset};
	inline rp2_pll usb_pll{0x4002c000, usb_pll_params, &usb_pll_reset};

	inline rp2_clk_controller clk_ctrl{0x40008000,
		rp2_clk_attachments{
			.adc_clk = rp2_clk_id::pll_usb,
			.usb_clk = rp2_clk_id::pll_usb,
			.rtc_clk = rp2_clk_id::pll_usb,
			.peri_clk = rp2_clk_id::sys,
			.sys_clk = rp2_clk_id::pll_sys,
			.ref_clk = rp2_clk_id::xosc
		}};

	inline rp2_clk adc_clk{&clk_ctrl, &usb_pll, 48_MHz, rp2_clk_id::adc};
	inline rp2_clk usb_clk{&clk_ctrl, &usb_pll, 48_MHz, rp2_clk_id::usb};
	inline rp2_clk rtc_clk{&clk_ctrl, &usb_pll, 46875, rp2_clk_id::rtc};
	inline rp2_clk sys_clk{&clk_ctrl, &sys_pll, 125_MHz, rp2_clk_id::sys};
	inline rp2_clk ref_clk{&clk_ctrl, &xosc, 12_MHz, rp2_clk_id::ref};
	inline rp2_clk peri_clk{&clk_ctrl, &sys_clk, 125_MHz, rp2_clk_id::peri};

} // namespace tart
