#include <tart/drivers/clk/rp2-clk.hpp>
#include <frg/tuple.hpp>
#include <assert.h>

namespace {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> ctrl{0x00};
		inline constexpr arch::scalar_register<uint32_t> div{0x04};
		inline constexpr arch::scalar_register<uint32_t> selected{0x08};

		inline constexpr arch::mem_space clk_space(arch::mem_space space, tart::rp2_clk_id id) {
			switch (id) {
				using enum tart::rp2_clk_id;

				case adc: return space.subspace(0x60);
				case usb: return space.subspace(0x54);
				case rtc: return space.subspace(0x6c);
				case peri: return space.subspace(0x48);
				case sys: return space.subspace(0x3c);
				case ref: return space.subspace(0x30);

				default: assert(!"Unknown clock ID");
			}
		}
	} // namespace reg

	namespace ctrl {
		inline constexpr arch::field<uint32_t, bool> enable{11, 1};
		inline constexpr arch::field<uint32_t, uint8_t> auxsrc{5, 3};
		inline constexpr arch::field<uint32_t, uint8_t> src{0, 2};
	} // namespace ctrl

	inline constexpr bool has_glmux(tart::rp2_clk_id id) {
		using enum tart::rp2_clk_id;

		return id == ref || id == sys;
	}

	inline constexpr uint8_t generic_clk_att(
			tart::rp2_clk_id attachment) {
		switch (attachment) {
			using enum tart::rp2_clk_id;

			case pll_usb: return 0;
			case pll_sys: return 1;
			// ROSC skipped
			case xosc: return 3;
			// GPIN 0-1 skipped

			default: assert(!"Unknown clock ID");
		}

		return 0;
	}

	inline constexpr uint8_t peri_clk_att(
			tart::rp2_clk_id attachment) {
		switch (attachment) {
			using enum tart::rp2_clk_id;

			case sys: return 0;
			case pll_usb: return 1;
			case pll_sys: return 2;
			// ROSC skipped
			case xosc: return 4;
			// GPIN 0-1 skipped

			default: assert(!"Unknown clock ID");
		}

		return 0;
	}

	inline constexpr frg::tuple<uint8_t, uint8_t> ref_clk_att(
			tart::rp2_clk_id attachment) {
		switch (attachment) {
			using enum tart::rp2_clk_id;

			// ROSC skipped
			case pll_usb: return {1, 0};
			// GPIN 0-1 skipped
			case xosc: return {2, 0};

			default: assert(!"Unknown clock ID");
		}

		return {0, 0};
	}

	inline constexpr frg::tuple<uint8_t, uint8_t> determine_src_aux(
			const tart::rp2_clk *clk,
			const tart::rp2_clk_attachments &attachments) {
		switch (clk->id()) {
			using enum tart::rp2_clk_id;

			case adc:
				return {0, generic_clk_att(attachments.adc_clk)};
			case usb:
				return {0, generic_clk_att(attachments.usb_clk)};
			case rtc:
				return {0, generic_clk_att(attachments.rtc_clk)};
			case peri:
				return {0, peri_clk_att(attachments.rtc_clk)};
			case sys:
				if (attachments.sys_clk == ref)
					return {0, 0};
				return {1, generic_clk_att(attachments.sys_clk)};
			case ref:
				return ref_clk_att(attachments.ref_clk);

			default: assert(!"Unknown clock ID");
		}

		return {0, 0};
	}
} // namespace anonymous

namespace tart {

//	void set_source(uintptr_t clock, uint8_t src) {
//		auto space = clocks_space.subspace(clock);
//		space.store(reg::ctrl, space.load(reg::ctrl)
//				/ ctrl::src(src));

//		while (!(space.load(reg::selected) & (1 << src)))
//			;
//	}

void rp2_clk_controller::start(const rp2_clk *clk) {
	auto space = reg::clk_space(space_, clk->id());
	auto [src, aux] = determine_src_aux(clk, attachments_);

	// The clock divider is in 24.8 fixed point format
	uint32_t div = (clk->freq() << 8) / clk->parent_clk()->freq();

	// Avoid overspeed if old configuration is faster
	if (div > space.load(reg::div))
		space.store(reg::div, div);

	if (has_glmux(clk->id()) && src != 0) {
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

	if (has_glmux(clk->id())) {
		space.store(reg::ctrl, space.load(reg::ctrl)
			/ ctrl::src(src));

		while (!(space.load(reg::selected) & (1 << src)))
			;
	}

	space.store(reg::ctrl, space.load(reg::ctrl)
			/ ctrl::enable(true));

	space.store(reg::div, div);
}

void rp2_clk_controller::stop(const rp2_clk *clk) {
	auto space = reg::clk_space(space_, clk->id());

	space.store(reg::ctrl, space.load(reg::ctrl)
			/ ctrl::enable(false));
}

} // namespace tart
