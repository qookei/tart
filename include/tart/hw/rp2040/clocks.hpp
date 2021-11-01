#pragma once

#include <arch/register.hpp>

namespace tart::hw::clocks {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> ctrl{0x00};
		inline constexpr arch::scalar_register<uint32_t> div{0x04};
		inline constexpr arch::scalar_register<uint32_t> selected{0x08};

		inline constexpr ptrdiff_t gpout0 = 0x00;
		inline constexpr ptrdiff_t gpout1 = 0x0c;
		inline constexpr ptrdiff_t gpout2 = 0x18;
		inline constexpr ptrdiff_t gpout3 = 0x24;
		inline constexpr ptrdiff_t ref = 0x30;
		inline constexpr ptrdiff_t sys = 0x3c;
		inline constexpr ptrdiff_t peri = 0x48;
		inline constexpr ptrdiff_t usb = 0x54;
		inline constexpr ptrdiff_t adc = 0x60;
		inline constexpr ptrdiff_t rtc = 0x6c;

		inline constexpr arch::bit_register<uint32_t> resus_ctrl{0x78};
		inline constexpr arch::bit_register<uint32_t> resus_status{0x7c};

		inline constexpr arch::scalar_register<uint32_t> enabled0{0xb0};
		inline constexpr arch::scalar_register<uint32_t> enabled1{0xb4};
	} // namespace reg

	namespace ctrl {
		inline constexpr arch::field<uint32_t, bool> nudge{20, 1};
		inline constexpr arch::field<uint32_t, uint8_t> phase{16, 2};
		inline constexpr arch::field<uint32_t, bool> enable{11, 1};
		inline constexpr arch::field<uint32_t, bool> kill{10, 1};
		inline constexpr arch::field<uint32_t, uint8_t> auxsrc{5, 3};
		inline constexpr arch::field<uint32_t, uint8_t> src{0, 2};
	} // namespace ctrl

	namespace src {
		inline constexpr uint8_t ref_xosc = 2;

		inline constexpr uint8_t sys_clock_aux = 1;
		inline constexpr uint8_t sys_pll_sys = 0;

		inline constexpr uint8_t peri_clock_sys = 0;

		inline constexpr uint8_t usb_pll_usb = 0;
		inline constexpr uint8_t adc_pll_usb = 0;
		inline constexpr uint8_t rtc_pll_usb = 0;

	} // namespace src
} // namespace tart::hw::clocks

namespace tart::hw::xosc {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> ctrl{0x00};
		inline constexpr arch::bit_register<uint32_t> status{0x04};
		inline constexpr arch::scalar_register<uint32_t> count{0x1c};
	} // namespace reg

	namespace ctrl {
		enum class enable_val {
			disable = 0xd1e,
			enable = 0xfab
		};

		enum class freq_range_val {
			one_fifteen_mhz = 0xaa0
		};

		inline constexpr arch::field<uint32_t, enable_val> enable{12, 12};
		inline constexpr arch::field<uint32_t, freq_range_val> freq_range{0, 12};
	} // namespace ctrl

	namespace status {
		inline constexpr arch::field<uint32_t, bool> stable{31, 1};
		inline constexpr arch::field<uint32_t, bool> badwrite{24, 1};
		inline constexpr arch::field<uint32_t, bool> enabled{12, 1};
	} // namespace status
} // namespace tart::hw::xosc

namespace tart::hw::pll {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> cs{0x00};
		inline constexpr arch::bit_register<uint32_t> pwr{0x04};
		inline constexpr arch::scalar_register<uint32_t> fbdiv_int{0x08};
		inline constexpr arch::bit_register<uint32_t> prim{0x0c};
	} // namespace reg

	namespace cs {
		inline constexpr arch::field<uint32_t, bool> lock{31, 1};
		inline constexpr arch::field<uint32_t, bool> bypass{8, 1};
		inline constexpr arch::field<uint32_t, uint8_t> refdiv{0, 6};
	} // namespace cs

	namespace pwr {
		inline constexpr arch::field<uint32_t, bool> vcopd{5, 1};
		inline constexpr arch::field<uint32_t, bool> postdivpd{3, 1};
		inline constexpr arch::field<uint32_t, bool> dsmpd{2, 1};
		inline constexpr arch::field<uint32_t, bool> pd{0, 1};
	} // namespace pwr

	namespace prim {
		inline constexpr arch::field<uint32_t, uint8_t> postdiv1{16, 3};
		inline constexpr arch::field<uint32_t, uint8_t> postdiv2{12, 3};
	}
} // namespace tart::hw::pll
