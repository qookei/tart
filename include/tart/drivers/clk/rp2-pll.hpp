#pragma once

#include <tart/drivers/reset.hpp>
#include <tart/drivers/clk.hpp>
#include <arch/mem_space.hpp>

namespace {
	// FIXME(qookie): This should not be here
	template <typename T>
	inline constexpr T abs(T a) {
		if (a < 0) return -a;
		else return a;
	}
} // namespace anonymous

namespace tart {

struct rp2_pll_params {
	int64_t actual_clk;
	uint32_t fbdiv;
	uint32_t postdiv1;
	uint32_t postdiv2;

	static constexpr rp2_pll_params compute(int64_t input_clk, int32_t target_clk,
			bool low_vco = false, int64_t vco_min = 400_MHz, int64_t vco_max = 1600_MHz) {
		constexpr uint32_t fbdiv_min = 16;
		constexpr uint32_t fbdiv_max = 320;

		int64_t best_margin = target_clk;

		rp2_pll_params best{};

		for (uint32_t fbdiv =
				low_vco
					? fbdiv_min
					: fbdiv_max;
				low_vco
					? fbdiv <= fbdiv_max
					: fbdiv >= fbdiv_min;
				low_vco
					? fbdiv++
					: fbdiv--) {
			int64_t vco = input_clk * fbdiv;

			if (vco < vco_min || vco > vco_max)
				continue;

			for (uint32_t pd2 = 1; pd2 <= 7; pd2++) {
				for (uint32_t pd1 = 1; pd1 <= 7; pd1++) {
					int64_t out = vco / pd1 / pd2;
					int64_t margin = abs(out - target_clk);

					if (margin < best_margin) {
						best = {out, fbdiv, pd1, pd2};
						best_margin = margin;
					}
				}
			}
		}

		return best;
	}
};

struct rp2_pll final : clk {
	rp2_pll(uintptr_t base, rp2_pll_params params, block_reset *reset)
	: clk{nullptr}, space_{base}, params_{params}, reset_{reset} { }

	virtual ~rp2_pll() = default;

	int64_t freq() const override { return params_.actual_clk; }
	void start() override;
	void stop() override;

private:
	// TODO(qookie): Remove the :: prefix once tart::arch is gone.
	::arch::mem_space space_;

	rp2_pll_params params_;
	block_reset *reset_;
};

} // namespace tart
