#pragma once

#include <arch/register.hpp>

namespace platform::flash::reg {
	static constexpr arch::bit_register<uint32_t> acr{0};
} // namespace platform::flash::reg

namespace platform::flash::acr {
	static constexpr arch::field<uint32_t, bool> pb_status{5, 1};
	static constexpr arch::field<uint32_t, bool> pb_enable{4, 1};
	static constexpr arch::field<uint32_t, bool> hca_enable{3, 1};
	static constexpr arch::field<uint32_t, uint8_t> ws{0, 3};
} // namespace platform::flash::acr
