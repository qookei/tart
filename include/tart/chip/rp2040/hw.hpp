#include <tart/drivers/reset/rp2-resets.hpp>
#include <tart/drivers/gpio/rp2-gpio.hpp>
#include <tart/drivers/uart/pl011.hpp>
#include <tart/drivers/clk/rp2-osc.hpp>
#include <tart/drivers/clk/rp2-pll.hpp>
#include <tart/drivers/clk/rp2-clk.hpp>

namespace tart {
	inline constinit rp2_resets resets{0x4000c000};
	inline constinit block_reset sys_pll_reset{&resets, 12, 0};
	inline constinit block_reset usb_pll_reset{&resets, 13, 0};
	inline constinit block_reset uart0_reset{&resets, 22, 0};
	inline constinit block_reset uart1_reset{&resets, 23, 0};
	inline constinit block_reset io_bank0_reset{&resets, 5, 0};
	inline constinit block_reset pads_bank0_reset{&resets, 8, 0};

	inline constinit rp2_gpio gpio{0xd0000000, 0x4001c000, 0x40014000, &io_bank0_reset, &pads_bank0_reset};
	inline constinit gpio_pin uart0_tx_pin{&gpio, 0, 2, gpio_flags::output};
	inline constinit gpio_pin uart0_rx_pin{&gpio, 1, 2, gpio_flags::input | gpio_flags::pull_up};
	inline constinit gpio_pin led_pin{&gpio, 25, 5, gpio_flags::output};

	inline constinit pl011_uart uart0{0x40034000, {&uart0_rx_pin, &uart0_tx_pin}, &uart0_reset};

	inline constinit rp2_xosc xosc{0x40024000, 12_MHz};

	inline constexpr rp2_pll_params sys_pll_params =
		rp2_pll_params::compute(12_MHz, 125_MHz);

	inline constexpr rp2_pll_params usb_pll_params =
		rp2_pll_params::compute(12_MHz, 48_MHz);

	inline constinit rp2_pll sys_pll{0x40028000, sys_pll_params, &sys_pll_reset};
	inline constinit rp2_pll usb_pll{0x4002c000, usb_pll_params, &usb_pll_reset};

	inline constinit rp2_clk_controller clk_ctrl{0x40008000,
		rp2_clk_attachments{
			.adc_clk = rp2_clk_id::pll_usb,
			.usb_clk = rp2_clk_id::pll_usb,
			.rtc_clk = rp2_clk_id::pll_usb,
			.peri_clk = rp2_clk_id::sys,
			.sys_clk = rp2_clk_id::pll_sys,
			.ref_clk = rp2_clk_id::xosc
		}};

	inline constinit rp2_clk adc_clk{&clk_ctrl, &usb_pll, 48_MHz, rp2_clk_id::adc};
	inline constinit rp2_clk usb_clk{&clk_ctrl, &usb_pll, 48_MHz, rp2_clk_id::usb};
	inline constinit rp2_clk rtc_clk{&clk_ctrl, &usb_pll, 46875, rp2_clk_id::rtc};
	inline constinit rp2_clk sys_clk{&clk_ctrl, &sys_pll, 125_MHz, rp2_clk_id::sys};
	inline constinit rp2_clk ref_clk{&clk_ctrl, &xosc, 12_MHz, rp2_clk_id::ref};
	inline constinit rp2_clk peri_clk{&clk_ctrl, &sys_clk, 125_MHz, rp2_clk_id::peri};

	inline void hw_initialize() {
		clk_ctrl.preinit_clks_to_internal();

		xosc.start();
		sys_pll.start();
		usb_pll.start();

		ref_clk.start();
		sys_clk.start();
		usb_clk.start();
		adc_clk.start();
		rtc_clk.start();
		peri_clk.start();

		gpio.init();

		led_pin.configure();
		led_pin.set(true);

		uart0.configure(b115200_8n1);
	}
} // namespace tart
