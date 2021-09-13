#pragma once

#include <arch/mem_space.hpp>

namespace platform::clocks {
	inline constexpr arch::mem_space clocks_space{0x40008000};

	// Sets up REF=12MHz, USB,ADC=48MHz, PERI,SYS=125MHz, RTC=~46KHz
	void setup_ext_12mhz();
} // namesapce platform::clocks

namespace platform::xosc {
	inline constexpr arch::mem_space xosc_space{0x40024000};

	void init();
	void busy_wait(uint8_t cycles);
} // namespace platform::xosc

namespace platform::pll {
	inline constexpr arch::mem_space pll_sys_space{0x40028000};
	inline constexpr arch::mem_space pll_usb_space{0x4002c000};

	void init_sys_125mhz();
	void init_usb_48mhz();
} // namespace platform::pll
