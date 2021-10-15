#include <tart/chip/irq.hpp>
#include <tart/lib/logger.hpp>

namespace tart::chip {

void handle_irq(void *, size_t irq) {
	lib::log("tart: irq %lu\r\n", irq);
}

uint32_t get_core_freq() {
	return 125000000; // TODO: don't hardcode this
}

} // namespace tart::chip
