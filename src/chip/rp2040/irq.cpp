#include <tart/chip/irq.hpp>
#include <tart/lib/logger.hpp>

namespace tart::chip {

void handle_irq(void *, size_t irq) {
	lib::log("tart: irq %lu\r\n", irq);
}

} // namespace tart::chip
