#pragma once

#include "usart.hpp"

#include <stdint.h>
#include <stddef.h>

#include <arch/register.hpp>

namespace platform::usart::reg {
	static constexpr arch::bit_register<uint32_t> status{0x00};
	static constexpr arch::scalar_register<uint32_t> data{0x04};
	static constexpr arch::scalar_register<uint32_t> baud{0x08};
	static constexpr arch::bit_register<uint32_t> cr1{0x0C};
	static constexpr arch::bit_register<uint32_t> cr2{0x10};
	static constexpr arch::bit_register<uint32_t> cr3{0x14};
} // namespace platform::usart::reg

namespace platform::usart::status {
	static constexpr arch::field<uint32_t, bool> transmit_empty{7, 1};
	static constexpr arch::field<uint32_t, bool> transmission_complete{6, 1};
	static constexpr arch::field<uint32_t, bool> read_full{5, 1};
	static constexpr arch::field<uint32_t, bool> overrun_err{3, 1};
	static constexpr arch::field<uint32_t, bool> noise_err{2, 1};
	static constexpr arch::field<uint32_t, bool> framing_err{1, 1};
	static constexpr arch::field<uint32_t, bool> parity_err{0, 1};
} // namespace platform::usart::status

namespace platform::usart::cr1 {
	static constexpr arch::field<uint32_t, bool> usart_enable{13, 1};
	static constexpr arch::field<uint32_t, bool> word_length{12, 1};
	static constexpr arch::field<uint32_t, bool> parity_enable{10, 1};
	static constexpr arch::field<uint32_t, bool> parity{9, 1};
	static constexpr arch::field<uint32_t, bool> transmit_enable{3, 1};
	static constexpr arch::field<uint32_t, bool> receive_enable{2, 1};
} // namespace platform::usart::cr1

namespace platform::usart::cr2 {
	static constexpr arch::field<uint32_t, stop_bits> stop_bits{12, 2};
} // namespace platform::usart::cr2

namespace platform::usart::cr3 {
	static constexpr arch::field<uint32_t, bool> crs{9, 1};
	static constexpr arch::field<uint32_t, bool> rts{8, 1};
} // namespace platform::usart::cr3
