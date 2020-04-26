#pragma once

#include "spi.hpp"

#include <stdint.h>
#include <stddef.h>

#include <arch/register.hpp>

namespace platform::spi::reg {
	static constexpr arch::bit_register<uint16_t> status{0x08};
	static constexpr arch::scalar_register<uint16_t> data{0x0C};
	static constexpr arch::bit_register<uint16_t> cr1{0x00};
	static constexpr arch::bit_register<uint16_t> cr2{0x04};
} // namespace platform::spi::reg

namespace platform::spi::status {
	static constexpr arch::field<uint16_t, bool> transmit_empty{1, 1};
	static constexpr arch::field<uint16_t, bool> read_full{0, 1};
} // namespace platform::spi::status

namespace platform::spi::cr1 {
	static constexpr arch::field<uint16_t, bool> bidi_mode{15, 1};
	static constexpr arch::field<uint16_t, bool> bidi_oe{14, 1};
	static constexpr arch::field<uint16_t, bool> crc_enable{13, 1};
	static constexpr arch::field<uint16_t, bool> crc_next{12, 1};
	static constexpr arch::field<uint16_t, size> word_length{11, 1};
	static constexpr arch::field<uint16_t, bool> rx_only{10, 1};
	static constexpr arch::field<uint16_t, bool> ssm{9, 1};
	static constexpr arch::field<uint16_t, bool> ssi{8, 1};
	static constexpr arch::field<uint16_t, bool> msb_first{7, 1};
	static constexpr arch::field<uint16_t, bool> enable{6, 1};
	static constexpr arch::field<uint16_t, uint8_t> clock_divisor{3, 3};
	static constexpr arch::field<uint16_t, bool> master{2, 1};
	static constexpr arch::field<uint16_t, bool> clock_polarity{1, 1};
	static constexpr arch::field<uint16_t, bool> clock_phase{0, 1};
} // namespace platform::spi::cr1

namespace platform::spi::cr2 {
	static constexpr arch::field<uint16_t, bool> ss_oe{2, 1};
} // namespace platform::spi::cr2
