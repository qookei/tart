#pragma once

#include <stdint.h>

namespace tart {
	struct clk {
		constexpr clk(const clk *parent = nullptr)
		: parent_clk_{parent} { }

		virtual int64_t freq() const = 0;
		virtual void start() = 0;
		virtual void stop() = 0;

		const clk *parent_clk() const {
			return parent_clk_;
		}

	protected:
		~clk() = default;
		const clk *parent_clk_;
	};

	struct fixed_clk final : clk {
		constexpr fixed_clk(int64_t freq)
		: clk{nullptr}, freq_{freq} { }

		int64_t freq() const override { return freq_; }
		void start() override { }
		void stop() override { }

	private:
		int64_t freq_;
	};

	constexpr uint64_t operator""_KHz(unsigned long long freq) {
		return freq * 1000ULL;
	}

	constexpr uint64_t operator""_MHz(unsigned long long freq) {
		return freq * 1000000ULL;
	}
} // namespace tart
