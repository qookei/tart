#include <tart/chip/irq.hpp>
#include <tart/log.hpp>
#include <tart/irq_service.hpp>

namespace tart::chip {

void handle_irq(void *, size_t irq) {
	info() << "tart: irq " << irq << frg::endlog;

	switch (irq) {
//		case 20: uart0.handle_irq(); break;
	}
}

uint32_t get_core_freq() {
	return 125000000; // TODO: don't hardcode this
}

} // namespace tart::chip
