#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/mem_space.hpp>

namespace platform::usart {
	inline constexpr arch::mem_space usart_space[2] = {
		0x40034000,
		0x40038000
	};

	void init(int nth, int baud);
	void send(int nth, uint8_t value);
	void send(int nth, const void *data, size_t size);

} // namespace platform::usart
