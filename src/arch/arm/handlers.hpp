#pragma once

#include <stddef.h>

namespace tart {

void nmi(void *ctx);
void hard_fault(void *ctx);
void mm_fault(void *ctx);
void bus_fault(void *ctx);
void usage_fault(void *ctx);
void sv_call(void *ctx);
void pend_sv_call(void *ctx);
void systick(void *ctx);

} // namespace tart
