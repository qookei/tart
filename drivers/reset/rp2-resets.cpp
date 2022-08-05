#include <tart/drivers/reset/rp2-resets.hpp>

namespace {
	namespace reg {
		inline constexpr arch::scalar_register<uint32_t> reset{0x00};
		inline constexpr arch::scalar_register<uint32_t> reset_done{0x08};
	} // namespace reg
} // namespace anonymous

namespace tart {
	void rp2_resets::reset(uint16_t block) {
		auto block_mask = 1 << block;

		auto orig = space_.load(reg::reset);
		space_.store(reg::reset, orig | block_mask);
		space_.store(reg::reset, orig & ~block_mask);

		while (!(space_.load(reg::reset_done) & block_mask))
			;
	}
} // namespace tart
