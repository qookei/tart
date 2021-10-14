#pragma once

#include <stddef.h>

namespace tart::chip {
#if defined(TART_CHIP_RP2040)
	inline constexpr size_t n_irqs = 26;
#elif defined(TART_CHIP_STM32F103X8)
	inline constexpr size_t n_irqs = 68;
#else
#error Unknown chip
#endif

	void handle_irq(void *ctx, size_t irq);
} // namespace tart::chip
