#pragma once

namespace tart::arch {
	inline void wait_for_irq() {
#if defined(__arm__)
		asm volatile ("wfi");
#else
#		error Unknown architecture
#endif
	}
} // namespace tart::arch
