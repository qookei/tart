#pragma once

#include <stdint.h>

namespace platform {
	template <typename F>
	void walk_stack(F functor) {
		uintptr_t *bp;
		asm volatile ("mov %[v], r7" : [v] "=r" (bp) : : "memory");

		// TODO: hardcoded region
		for (uintptr_t ip = bp[1];
				ip >= 0x8000000 && ip < 0x8010000 && bp;
				ip = bp[1],
				bp = reinterpret_cast<uintptr_t *>(bp[0])) {
			functor(ip);
		}
	}
} // namespace platform
