#pragma once

#include <tart/drivers/uart.hpp>
#include <tart/drivers/reset.hpp>

#include <stdint.h>
#include <stddef.h>

#include <arch/mem_space.hpp>

namespace tart {
	struct pl011_uart final : uart {
		// TODO(qookie): Add clock argument
		constexpr pl011_uart(uintptr_t base, uart_gpios gpios, block_reset *reset)
		: uart{gpios}, space_{base}, reset_{reset} { }

		~pl011_uart() = default;

		void configure(uart_params params) override;

		size_t try_write(const void *data, size_t size) override;
		size_t try_read(void *data, size_t size) override;

	private:
		bool tx_full() const;
		bool rx_empty() const;

		::arch::mem_space space_;
		block_reset *reset_;
	};
} // namespace uart
