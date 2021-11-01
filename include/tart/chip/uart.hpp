#pragma once

#include <stddef.h>

#include <arch/mem_space.hpp>

namespace tart::chip {
	struct uart {
		constexpr uart(int nth, uintptr_t base)
		: nth_{nth}, space_{base} { }

		void init(int baud);
		void send_sync(const void *data, size_t size);
		void send_sync(const char *data);

		bool tx_full() const;
		bool rx_empty() const;

		int nth() const {
			return nth_;
		}

	private:
		int nth_;
		arch::mem_space space_;
	};

#if defined (TART_CHIP_RP2040)
	inline constinit uart uart0{0, 0x40034000};
	inline constinit uart uart1{1, 0x40038000};
#endif

} // namespace tart::chip
