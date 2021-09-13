#pragma once

#include <arch/mem_space.hpp>

namespace platform::resets {
	static constexpr arch::mem_space resets_space{0x4000c000};

	enum class periph {
		adc = 0,
		busctrl,
		dma,
		i2c0,
		i2c1,
		io_bank0,
		io_qspi,
		jtag,
		pads_bank0,
		pads_qspi,
		pio0,
		pio1,
		pll_sys,
		pll_usb,
		pwm,
		rtc,
		spi0,
		spi1,
		syscfg,
		sysinfo,
		tbman,
		timer,
		uart0,
		uart1,
		usbctrl
	};

	void reset(periph p);
} // namespace platform::resets
