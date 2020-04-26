#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/arm/mem_space.hpp>

// TODO: this only allows you to set up a SPI master,
//       maybe implement SPI slave mode as well.

// TODO: the STM32F1xx line of chips has a builtin CRC calculator for SPI
//       transfers, maybe support that.

namespace platform::spi {
	static constexpr arch::mem_space spi_space[2] = {
		0x40013000,
		0x40003800
	};

	enum class size {
		eight,
		sixteen
	};

	void init(int nth, int clock_divisor, int mode, bool msb, size s);

	uint16_t transfer(int nth, uint16_t in);

} // namespace platform::spi
