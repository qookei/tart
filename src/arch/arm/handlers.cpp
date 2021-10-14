#include "handlers.hpp"

#include <stdint.h>

#include <tart/lib/logger.hpp>

namespace tart {

struct irq_state {
	uintptr_t r0, r1, r2, r3, r12;
	uintptr_t lr, pc, psr;

	void log() {
		lib::log("r0: 0x%08x, r1: 0x%08x, r2: 0x%08x, r3: 0x%08x, r12: 0x%08x\r\n", r0, r1, r2, r3, r12);
		lib::log("lr = 0x%08x, pc = 0x%08x, psr = 0x%08x\r\n", lr, pc, psr);
	}
};

void nmi(void *ctx) {
	lib::log("tart: unexpected nmi\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected nmi\r\n");
}

void hard_fault(void *ctx) {
	lib::log("tart: unexpected hard fault\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected hard fault\r\n");
}

void mm_fault(void *ctx) {
	lib::log("tart: unexpected mm fault\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected mm fault\r\n");
}

void bus_fault(void *ctx) {
	lib::log("tart: unexpected bus fault\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected bus fault\r\n");
}

void usage_fault(void *ctx) {
	lib::log("tart: unexpected usage fault\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected usage fault\r\n");
}

void sv_call(void *ctx) {
	lib::log("tart: unexpected sv call\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected sv call\r\n");
}

void pend_sv_call(void *ctx) {
	lib::log("tart: unexpected pending sv call\r\n");
	static_cast<irq_state *>(ctx)->log();
	lib::panic("tart: unexpected pending sv call\r\n");
}

void systick(void *ctx) {
	lib::log("tart: unexpected systick\r\n");
	static_cast<irq_state *>(ctx)->log();
}

void handle_irq(void *ctx, size_t irq) {
	lib::log("tart: irq %lu\r\n", irq);
	static_cast<irq_state *>(ctx)->log();
}

} // namespace tart
