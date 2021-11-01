#include "handlers.hpp"
#include <tart/time.hpp>

#include <stdint.h>

#include <tart/log.hpp>

namespace tart {

struct irq_state {
	uintptr_t r0, r1, r2, r3, r12;
	uintptr_t lr, pc, psr;

	void log() {
		info() << "r0 = " << frg::hex_fmt{r0} << frg::endlog;
		info() << "r1 = " << frg::hex_fmt{r1} << frg::endlog;
		info() << "r2 = " << frg::hex_fmt{r2} << frg::endlog;
		info() << "r3 = " << frg::hex_fmt{r3} << frg::endlog;
		info() << "r12 = " << frg::hex_fmt{r12} << frg::endlog;
		info() << "lr = " << frg::hex_fmt{lr} << frg::endlog;
		info() << "pc = " << frg::hex_fmt{pc} << frg::endlog;
		info() << "psr = " << frg::hex_fmt{psr} << frg::endlog;
	}
};

void nmi(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected nmi" << frg::endlog;
}

void hard_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected hard fault" << frg::endlog;
}

void mm_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected mm fault" << frg::endlog;
}

void bus_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected bus fault" << frg::endlog;
}

void usage_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected usage fault" << frg::endlog;
}

void sv_call(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected sv call" << frg::endlog;
}

void pend_sv_call(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected pending sv call" << frg::endlog;
}

void systick(void *) {
	current_time_ = current_time_ + 1;
}

} // namespace tart
