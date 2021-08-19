#pragma once

#include <stdint.h>

#include <arch/register.hpp>

namespace platform::resets {
	namespace reg {
		inline constexpr arch::scalar_register<uint32_t> reset{0x00};
		inline constexpr arch::scalar_register<uint32_t> wdsel{0x04};
		inline constexpr arch::scalar_register<uint32_t> reset_done{0x08};
	} // namespace reg
} // namespace platform::resets

