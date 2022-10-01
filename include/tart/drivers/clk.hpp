#pragma once

#include <stdint.h>

namespace tart {
	struct clk {
		clk(clk *parent = nullptr)
		: parent_clk_{parent} { }

		virtual uint32_t freq() const = 0;
		virtual void start() = 0;
		virtual void stop() = 0;

		clk *parent_clk() const {
			return parent_clk_;
		}

	protected:
		virtual ~clk() = default;
		clk *parent_clk_;
	};

	struct fixed_clk final : clk {
		fixed_clk(uint32_t freq)
		: clk{nullptr}, freq_{freq} { }

		uint32_t freq() const override { return freq_; }
		void start() override { }
		void stop() override { }

	private:
		uint32_t freq_;
	};
} // namespace tart
