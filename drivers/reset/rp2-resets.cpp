#include <tart/drivers/reset/rp2-resets.hpp>

namespace {
	namespace reg {
		inline constexpr arch::bit_register<uint32_t> reset{0x00};
		inline constexpr arch::bit_register<uint32_t> reset_done{0x08};
	} // namespace reg
} // namespace anonymous

namespace tart {
	void rp2_resets::reset(uint16_t block) {
		arch::field<uint32_t, bool> block_f{block, 1};

		auto orig = space_.load(reg::reset);
		space_.store(reg::reset, orig / block_f(true));
		space_.store(reg::reset, orig / block_f(false));

		while (!(space_.load(reg::reset_done) & block_f))
			;
	}
} // namespace tart
