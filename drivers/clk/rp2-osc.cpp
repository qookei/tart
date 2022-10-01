#include <tart/drivers/clk/rp2-osc.hpp>
#include <assert.h>

namespace {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> ctrl{0x00};
		inline constexpr arch::bit_register<uint32_t> status{0x04};
		inline constexpr arch::scalar_register<uint32_t> count{0x1c};
	} // namespace reg

	namespace ctrl {
		inline constexpr uint16_t enable_v = 0xfab;
		inline constexpr uint16_t disable_v = 0xfab;

		inline constexpr uint16_t one_fifteen_mhz = 0xaa0;

		inline constexpr arch::field<uint32_t, uint16_t> enable{12, 12};
		inline constexpr arch::field<uint32_t, uint16_t> freq_range{0, 12};
	} // namespace ctrl

	namespace status {
		inline constexpr arch::field<uint32_t, bool> stable{31, 1};
	} // namespace status
} // namespace anonymous

namespace tart {

void rp2_xosc::start() {
	assert(freq_ >= 1000000 && freq_ <= 15000000);

	space_.store(reg::ctrl, ctrl::enable(ctrl::enable_v)
			| ctrl::freq_range(ctrl::one_fifteen_mhz));

	// Wait for XOSC to become stable
	while (!(space_.load(reg::status) & status::stable))
		;
}

void rp2_xosc::stop() {
	space_.store(reg::ctrl, ctrl::enable(ctrl::disable_v));
}

void rp2_xosc::busy_wait(uint8_t cycles) {
	space_.store(reg::count, cycles);

	while (space_.load(reg::count))
		;
}

} // namespace tart
