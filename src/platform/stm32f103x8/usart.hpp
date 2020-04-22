#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/arm/mem_space.hpp>

namespace platform::usart {
	static constexpr arch::mem_space usart_space[3] = {
		0x40013800,
		0x40004400,
		0x40004800
	};

	enum class parity {
		none,
		odd,
		even
	};

	enum class stop_bits {
		one,
		half,
		two,
		one_half
	};

	void init(int nth, int baud, parity par, stop_bits stop);
	void send(int nth, uint8_t value);
	void send(int nth, const void *data, size_t size);

} // namespace platform::usart
