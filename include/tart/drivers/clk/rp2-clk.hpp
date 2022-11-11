#pragma once

#include <tart/drivers/clk.hpp>
#include <arch/mem_space.hpp>

namespace tart {

// TODO(qookie): Support GPOUT clocks and GPIN sources

enum class rp2_clk_id {
	none,

	xosc,
	pll_usb,
	pll_sys,

	adc,
	usb,
	rtc,
	peri,
	sys,
	ref
};

struct rp2_clk;

struct rp2_clk_attachments {
	rp2_clk_id adc_clk = rp2_clk_id::none;
	rp2_clk_id usb_clk = rp2_clk_id::none;
	rp2_clk_id rtc_clk = rp2_clk_id::none;
	rp2_clk_id peri_clk = rp2_clk_id::none;
	rp2_clk_id sys_clk = rp2_clk_id::none;
	rp2_clk_id ref_clk = rp2_clk_id::none;
};

struct rp2_clk_controller final {
	rp2_clk_controller(uintptr_t base, rp2_clk_attachments attachments)
	: space_{base}, attachments_{attachments} { }

	void start(rp2_clk *clk);
	void stop(rp2_clk *clk);

private:
	// TODO(qookie): Remove the :: prefix once tart::arch is gone.
	::arch::mem_space space_;

	rp2_clk_attachments attachments_;
};

struct rp2_clk final : clk {
	rp2_clk(rp2_clk_controller *ctrl, clk *from, int64_t target_clk, rp2_clk_id id)
	: clk{from}, ctrl_{ctrl}, target_clk_{target_clk}, id_{id} { }

	virtual ~rp2_clk() = default;

	int64_t freq() const override {
		return target_clk_;
	}

	void start() override {
		ctrl_->start(this);
	}

	void stop() override {
		ctrl_->stop(this);
	}

	constexpr auto id() const {
		return id_;
	}

private:
	rp2_clk_controller *ctrl_;

	int64_t target_clk_;
	rp2_clk_id id_;
};

} // namespace tart
