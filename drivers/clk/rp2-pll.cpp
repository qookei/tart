#include <tart/drivers/clk/rp2-pll.hpp>

namespace {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> cs{0x00};
		inline constexpr arch::bit_register<uint32_t> pwr{0x04};
		inline constexpr arch::scalar_register<uint32_t> fbdiv_int{0x08};
		inline constexpr arch::bit_register<uint32_t> prim{0x0c};
	} // namespace reg

	namespace cs {
		inline constexpr arch::field<uint32_t, bool> lock{31, 1};
		inline constexpr arch::field<uint32_t, uint8_t> refdiv{0, 6};
	} // namespace cs

	namespace pwr {
		inline constexpr arch::field<uint32_t, bool> vco_off{5, 1};
		inline constexpr arch::field<uint32_t, bool> postdiv_off{3, 1};
		inline constexpr arch::field<uint32_t, bool> dsm_off{2, 1};
		inline constexpr arch::field<uint32_t, bool> off{0, 1};
	} // namespace pwr

	namespace prim {
		inline constexpr arch::field<uint32_t, uint8_t> postdiv1{16, 3};
		inline constexpr arch::field<uint32_t, uint8_t> postdiv2{12, 3};
	}
} // namespace anonymous

namespace tart {

void rp2_pll::start() {
	space_.store(reg::cs, cs::refdiv(1));
	space_.store(reg::fbdiv_int, params_.fbdiv);

	space_.store(reg::pwr, pwr::vco_off(false)
			| pwr::off(false)
			| pwr::postdiv_off(true)
			| pwr::dsm_off(true));

	// Wait for VCO to lock at the target frequency
	while (!(space_.load(reg::cs) & cs::lock))
		;

	space_.store(reg::prim, prim::postdiv1(params_.postdiv1)
			| prim::postdiv2(params_.postdiv2));

	space_.store(reg::pwr, space_.load(reg::pwr)
			/ pwr::postdiv_off(false));
}

void rp2_pll::stop() {
	space_.store(reg::pwr, pwr::vco_off(true)
			| pwr::off(true)
			| pwr::postdiv_off(true)
			| pwr::dsm_off(true));
}

} // namespace tart
