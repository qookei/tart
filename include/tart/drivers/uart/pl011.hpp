#pragma once

#include <tart/drivers/uart.hpp>
#include <tart/drivers/clk.hpp>
#include <tart/drivers/reset.hpp>

#include <stdint.h>
#include <stddef.h>

#include <arch/mem_space.hpp>

namespace tart {
	struct pl011_uart final : uart {
		constexpr pl011_uart(uintptr_t base, uart_gpios gpios, block_reset *reset, clk *clock)
		: uart{gpios}, space_{base}, reset_{reset}, clock_{clock} { }

		~pl011_uart() = default;

		void configure(uart_params params) override;

		size_t try_write(const void *data, size_t size) override;
		size_t try_read(void *data, size_t size) override;

	private:
		bool tx_full() const;
		bool rx_empty() const;

		::arch::mem_space space_;
		block_reset *reset_;
		clk *clock_;
	};
} // namespace uart
