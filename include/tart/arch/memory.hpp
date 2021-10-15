#pragma once

#include <stdint.h>

namespace tart::arch {
	void walk_stack(void (*fn)(uintptr_t, void *), void *ctx);

	template <typename F>
	void walk_stack(F functor) {
		walk_stack([] (uintptr_t addr, void *ctx) {
			auto &fn = *static_cast<F *>(ctx);
			fn(addr);
		}, &functor);
	}
} // namespace tart::arch
