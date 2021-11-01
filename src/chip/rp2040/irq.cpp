#include <tart/chip/irq.hpp>
#include <tart/log.hpp>

namespace tart::chip {

void handle_irq(void *, size_t irq) {
	info() << "tart: irq " << irq << "\r\n" << frg::endlog;
}

uint32_t get_core_freq() {
	return 125000000; // TODO: don't hardcode this
}

} // namespace tart::chip
