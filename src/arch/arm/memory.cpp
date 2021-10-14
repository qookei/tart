#include <tart/arch/memory.hpp>

namespace tart {
	void walk_stack(void (*fn)(uintptr_t, void *), void *ctx) {
		uintptr_t *bp;
		asm volatile ("mov %[v], r7" : [v] "=r" (bp) : : "memory");

		// TODO: hardcoded region
		for (uintptr_t ip = bp[1];
				ip >= 0x10000000 && ip < 0x20000000 && (uintptr_t)bp >= 0x20000000 && (uintptr_t)bp <= 0x20042000;
				ip = bp[1],
				bp = reinterpret_cast<uintptr_t *>(bp[0])) {
			fn(ip, ctx);
		}
	}
} // namespace tart
