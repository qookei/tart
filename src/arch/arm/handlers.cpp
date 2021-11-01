#include "handlers.hpp"
#include <tart/time.hpp>

#include <stdint.h>

#include <tart/log.hpp>

namespace tart {

struct irq_state {
	uintptr_t r0, r1, r2, r3, r12;
	uintptr_t lr, pc, psr;

	void log() {
		info() << "r0 = " << frg::hex_fmt{r0} << "\r\n" << frg::endlog;
		info() << "r1 = " << frg::hex_fmt{r1} << "\r\n" << frg::endlog;
		info() << "r2 = " << frg::hex_fmt{r2} << "\r\n" << frg::endlog;
		info() << "r3 = " << frg::hex_fmt{r3} << "\r\n" << frg::endlog;
		info() << "r12 = " << frg::hex_fmt{r12} << "\r\n" << frg::endlog;
		info() << "lr = " << frg::hex_fmt{lr} << "\r\n" << frg::endlog;
		info() << "pc = " << frg::hex_fmt{pc} << "\r\n" << frg::endlog;
		info() << "psr = " << frg::hex_fmt{psr} << "\r\n" << frg::endlog;
	}
};

void nmi(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected nmi\r\n" << frg::endlog;
}

void hard_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected hard fault\r\n" << frg::endlog;
}

void mm_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected mm fault\r\n" << frg::endlog;
}

void bus_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected bus fault\r\n" << frg::endlog;
}

void usage_fault(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected usage fault\r\n" << frg::endlog;
}

void sv_call(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected sv call\r\n" << frg::endlog;
}

void pend_sv_call(void *ctx) {
	static_cast<irq_state *>(ctx)->log();
	fatal() << "tart: unexpected pending sv call\r\n" << frg::endlog;
}

void systick(void *) {
	current_time_ = current_time_ + 1;
}

} // namespace tart
