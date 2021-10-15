#pragma once

#include <arch/register.hpp>
#include <stdint.h>

namespace tart::arch::systick {
	namespace reg {
		inline constexpr ::arch::bit_register<uint32_t> csr{0x00};
		inline constexpr ::arch::scalar_register<uint32_t> rvr{0x04};
		inline constexpr ::arch::scalar_register<uint32_t> cvr{0x08};
	} // namespace reg

	namespace csr {
		inline constexpr ::arch::field<uint32_t, bool> countflag{16, 1};
		inline constexpr ::arch::field<uint32_t, bool> clksource{2, 1};
		inline constexpr ::arch::field<uint32_t, bool> tickint{1, 1};
		inline constexpr ::arch::field<uint32_t, bool> enable{0, 1};
	} // namespace ctrl
} // namespace tart::arch::systick
