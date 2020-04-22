#pragma once

#include "gpio.hpp"

#include <stdint.h>
#include <stddef.h>

#include <arch/register.hpp>

namespace platform::gpio::reg {
	static constexpr arch::bit_register<uint32_t> cr_low{0x00};
	static constexpr arch::bit_register<uint32_t> cr_high{0x04};
	static constexpr arch::scalar_register<uint32_t> input_data{0x08};
	static constexpr arch::scalar_register<uint32_t> output_data{0x0C};
	static constexpr arch::scalar_register<uint32_t> bit_set_reset{0x10};
	static constexpr arch::scalar_register<uint32_t> bit_reset{0x14};
} // namespace platform::gpio::reg

namespace platform::gpio::cr {
	static constexpr arch::field<uint32_t, mode> pin_mode[8] = {
		arch::field<uint32_t, mode>{0, 2},
		arch::field<uint32_t, mode>{4, 2},
		arch::field<uint32_t, mode>{8, 2},
		arch::field<uint32_t, mode>{12, 2},
		arch::field<uint32_t, mode>{16, 2},
		arch::field<uint32_t, mode>{20, 2},
		arch::field<uint32_t, mode>{24, 2},
		arch::field<uint32_t, mode>{28, 2},
	};

	static constexpr arch::field<uint32_t, conf> pin_conf[8] = {
		arch::field<uint32_t, conf>{2, 2},
		arch::field<uint32_t, conf>{6, 2},
		arch::field<uint32_t, conf>{10, 2},
		arch::field<uint32_t, conf>{14, 2},
		arch::field<uint32_t, conf>{18, 2},
		arch::field<uint32_t, conf>{22, 2},
		arch::field<uint32_t, conf>{26, 2},
		arch::field<uint32_t, conf>{30, 2},
	};
} // namespace platform::gpio::cr
