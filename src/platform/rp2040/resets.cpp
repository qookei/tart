#include <tart/chip/rp2040/resets.hpp>
#include <tart/chip/rp2040/reg/resets.hpp>

namespace platform::resets {
	void reset(periph p) {
		auto orig = resets_space.load(reg::reset);
		auto p_v = static_cast<uint32_t>(p);
		resets_space.store(reg::reset, orig | (1 << p_v));
		resets_space.store(reg::reset, orig & ~(1 << p_v));

		while (!(resets_space.load(reg::reset_done) & (1 << p_v)))
			;
	}
} // namespace platform::resets
