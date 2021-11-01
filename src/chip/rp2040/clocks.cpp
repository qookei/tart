#include <tart/chip/rp2040/clocks.hpp>
#include <tart/hw/rp2040/clocks.hpp>

#include <tart/chip/rp2040/resets.hpp>

namespace platform::clocks {
	using namespace tart::hw::clocks;
	inline constexpr uint32_t mhz = 1000000;

	constexpr bool has_glmux(uintptr_t clock) {
		return clock == reg::ref || clock == reg::sys;
	}

	void set_source(uintptr_t clock, uint8_t src) {
		auto space = clocks_space.subspace(clock);
		space.store(reg::ctrl, space.load(reg::ctrl)
				/ ctrl::src(src));

		while (!(space.load(reg::selected) & (1 << src)))
			;
	}

	void setup_clock(uintptr_t clock, uint8_t src, uint8_t aux,
			uint32_t src_freq, uint32_t freq) {
		auto space = clocks_space.subspace(clock);

		uint32_t div = ((uint64_t) src_freq << 8) / freq;

		if (div > space.load(reg::div))
			space.store(reg::div, div);

		if (has_glmux(clock) && src != 0) {
			space.store(reg::ctrl, space.load(reg::ctrl)
					/ ctrl::src(0));

			while (!(space.load(reg::selected) & 1))
				;
		} else {
			space.store(reg::ctrl, space.load(reg::ctrl)
				/ ctrl::enable(false));
		}

		space.store(reg::ctrl, space.load(reg::ctrl)
			/ ctrl::auxsrc(aux));

		if (has_glmux(clock)) {
			space.store(reg::ctrl, space.load(reg::ctrl)
				/ ctrl::src(src));

			while (!(space.load(reg::selected) & (1 << src)))
				;
		}

		space.store(reg::ctrl, space.load(reg::ctrl)
				/ ctrl::enable(true));

		space.store(reg::div, div);
	}

	void setup_ext_12mhz() {
		xosc::init();

		set_source(reg::sys, 0);
		set_source(reg::ref, 0);

		pll::init_usb_48mhz();
		pll::init_sys_125mhz();

		setup_clock(reg::ref, src::ref_xosc, 0, 12000000, 12000000);
		setup_clock(reg::sys, src::sys_clock_aux, src::sys_pll_sys,
				125000000, 125000000);
		setup_clock(reg::usb, 0, src::usb_pll_usb, 48000000, 48000000);
		setup_clock(reg::adc, 0, src::adc_pll_usb, 48000000, 48000000);
		setup_clock(reg::rtc, 0, src::rtc_pll_usb, 48000000, 46875);
		setup_clock(reg::peri, 0, src::peri_clock_sys,
				125000000, 125000000);
	}
} // namespace platform::clocks

namespace platform::xosc {
	using namespace tart::hw::xosc;
	void init() {
		xosc_space.store(reg::ctrl, ctrl::enable(ctrl::enable_val::enable)
				| ctrl::freq_range(ctrl::freq_range_val::one_fifteen_mhz));

		// Wait for XOSC to become stable
		while (!(xosc_space.load(reg::status) & status::stable))
			;
	}

	void busy_wait(uint8_t cycles) {
		xosc_space.store(reg::count, cycles);

		while (xosc_space.load(reg::count))
			;
	}
} // namespace platform::xosc

namespace platform::pll {
	using namespace tart::hw::pll;

	struct cfg {
		uint32_t refdiv;
		uint32_t fbdiv;
		uint32_t pdiv1, pdiv2;
	};

	inline constexpr cfg sys_cfg = {
		.refdiv = 1, .fbdiv = 125,
		.pdiv1 = 6, .pdiv2 = 2
	};

	inline constexpr cfg usb_cfg = {
		.refdiv = 1, .fbdiv = 40,
		.pdiv1 = 5, .pdiv2 = 2
	};

	void init_pll(const arch::mem_space &space, const cfg &c) {
		space.store(reg::cs, cs::refdiv(c.refdiv));
		space.store(reg::fbdiv_int, c.fbdiv);

		space.store(reg::pwr, pwr::vcopd(false)
				| pwr::pd(false)
				| pwr::postdivpd(true)
				| pwr::dsmpd(true));

		while (!(space.load(reg::cs) & cs::lock))
			;

		space.store(reg::prim, prim::postdiv1(c.pdiv1)
				| prim::postdiv2(c.pdiv2));

		space.store(reg::pwr, space.load(reg::pwr)
				/ pwr::postdivpd(false));
	}

	void init_sys_125mhz() {
		resets::reset(resets::periph::pll_sys);
		init_pll(pll_sys_space, sys_cfg);
	}

	void init_usb_48mhz() {
		resets::reset(resets::periph::pll_usb);
		init_pll(pll_usb_space, usb_cfg);
	}
} // namespace platform::xosc
