#pragma once

#include <tart/drivers/clk.hpp>
#include <arch/mem_space.hpp>

namespace tart {

struct rp2_xosc final : clk {
	rp2_xosc(uintptr_t base, int64_t freq)
	: clk{nullptr}, freq_{freq}, space_{base} { }

	virtual ~rp2_xosc() = default;

	int64_t freq() const override { return freq_; }
	void start() override;
	void stop() override;

	void busy_wait(uint8_t cycles);

private:
	int64_t freq_;

	// TODO(qookie): Remove the :: prefix once tart::arch is gone.
	::arch::mem_space space_;
};

} // namespace tart
