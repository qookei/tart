#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/register.hpp>

namespace tart::hw::uart {
	namespace reg {
		inline constexpr arch::scalar_register<uint32_t> data{0x00};
		inline constexpr arch::bit_register<uint32_t> status{0x18};
		inline constexpr arch::scalar_register<uint32_t> i_baud{0x24};
		inline constexpr arch::scalar_register<uint32_t> f_baud{0x28};
		inline constexpr arch::bit_register<uint32_t> control{0x30};
		inline constexpr arch::bit_register<uint32_t> line_control{0x2c};
		inline constexpr arch::bit_register<uint32_t> irq_mask{0x38};
		inline constexpr arch::bit_register<uint32_t> irq_clear{0x44};
		inline constexpr arch::bit_register<uint32_t> irq_status{0x40};
	} // namespace reg

	namespace status {
		inline constexpr arch::field<uint32_t, bool> tx_full{5, 1};
		inline constexpr arch::field<uint32_t, bool> rx_empty{4, 1};
	} // namespace status

	namespace control {
		inline constexpr arch::field<uint32_t, bool> rx_en{9, 1};
		inline constexpr arch::field<uint32_t, bool> tx_en{8, 1};
		inline constexpr arch::field<uint32_t, bool> uart_en{0, 1};
	} // namespace control

	namespace line_control {
		inline constexpr arch::field<uint32_t, uint8_t> word_len{5, 2};
		inline constexpr arch::field<uint32_t, bool> fifo_en{4, 1};
	} // namespace line_control

	namespace irqs {
		inline constexpr arch::field<uint32_t, bool> overrun_err{10, 1};
		inline constexpr arch::field<uint32_t, bool> break_err{9, 1};
		inline constexpr arch::field<uint32_t, bool> parity_err{8, 1};
		inline constexpr arch::field<uint32_t, bool> frame_err{7, 1};
		inline constexpr arch::field<uint32_t, bool> rx_timeout{6, 1};
		inline constexpr arch::field<uint32_t, bool> tx{5, 1};
		inline constexpr arch::field<uint32_t, bool> rx{4, 1};
	} // namespace irq_mask
} // namespace tart::hw::uart
