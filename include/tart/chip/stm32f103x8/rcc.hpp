#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/arm/mem_space.hpp>

namespace platform::rcc {
	static constexpr arch::mem_space rcc_space{0x40021000};

	enum class periph {
		afio,

		gpio_a,
		gpio_b,
		gpio_c,
		gpio_d,
		gpio_e,

		spi1,
		spi2,

		i2c1,
		i2c2,

		dma1,
		dma2,

		usart1,
		usart2,
		usart3,
	};

	void clock_setup_ext_8mhz_72mhz();
	void set_periph_enable(periph p, bool enabled);
} // namespace platform::rcc
