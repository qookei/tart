#pragma once

#include <stdint.h>
#include <stddef.h>

#include <arch/register.hpp>

namespace gpio::reg {
	static constexpr arch::bit_register<uint32_t> cr_low{0x00};
	static constexpr arch::bit_register<uint32_t> cr_high{0x04};
	static constexpr arch::scalar_register<uint32_t> input_data{0x08};
	static constexpr arch::scalar_register<uint32_t> output_data{0x0C};
	static constexpr arch::scalar_register<uint32_t> bit_set_reset{0x10};
	static constexpr arch::scalar_register<uint32_t> bit_reset{0x14};
} // namespace gpio::reg

namespace gpio::cr {
	static constexpr arch::field<uint32_t, uint8_t> pin_mode[8] = {
		arch::field<uint32_t, uint8_t>{0, 2},
		arch::field<uint32_t, uint8_t>{4, 2},
		arch::field<uint32_t, uint8_t>{8, 2},
		arch::field<uint32_t, uint8_t>{12, 2},
		arch::field<uint32_t, uint8_t>{16, 2},
		arch::field<uint32_t, uint8_t>{20, 2},
		arch::field<uint32_t, uint8_t>{24, 2},
		arch::field<uint32_t, uint8_t>{28, 2},
	};

	static constexpr arch::field<uint32_t, uint8_t> pin_conf[8] = {
		arch::field<uint32_t, uint8_t>{2, 2},
		arch::field<uint32_t, uint8_t>{6, 2},
		arch::field<uint32_t, uint8_t>{10, 2},
		arch::field<uint32_t, uint8_t>{14, 2},
		arch::field<uint32_t, uint8_t>{18, 2},
		arch::field<uint32_t, uint8_t>{22, 2},
		arch::field<uint32_t, uint8_t>{26, 2},
		arch::field<uint32_t, uint8_t>{30, 2},
	};
} // namespace gpio::cr
