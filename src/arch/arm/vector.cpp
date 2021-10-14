#include <tart/arch/init.hpp>

#include <stdint.h>
#include <stddef.h>

#include <frg/array.hpp>

#include "handlers.hpp"

using vec_fn = void (*)();

struct vector_table {
	void *stack;

	vec_fn reset;
	vec_fn nmi;
	vec_fn hard_fault;
	vec_fn mm_fault;
	vec_fn bus_fault;
	vec_fn usage_fault;
	uintptr_t reserved1[4] {};
	vec_fn sv_call;
	uintptr_t reserved2[2] {};
	vec_fn pend_sv_call;
	vec_fn systick;

	// TODO: get irq count and handler pointers from chip
	frg::array<vec_fn, 68> irqs;
};

template <void (*Fn)(void *)>
[[gnu::naked]] void vector_fn() {
	asm volatile ("mov r0, sp\n\tbx %0" :: "r"(Fn) : "r0");
}

template <size_t N>
void irq_handler(void *ctx) {
	tart::handle_irq(ctx, N);
}

template <size_t N>
frg::array<vec_fn, N> make_irq_handlers() {
	return [] <size_t ...I>(std::index_sequence<I...>) {
		return frg::array<vec_fn, N>{&vector_fn<irq_handler<I>>...};
	} (std::make_index_sequence<N>{});
}

extern "C" char core0_stack[];

[[gnu::section(".vectors"), gnu::used]]
vector_table tart_vector_table__ = {
	.stack = core0_stack,
	.reset = reset_handler,
	.nmi = &vector_fn<tart::nmi>,
	.hard_fault = &vector_fn<tart::hard_fault>,
	.mm_fault = &vector_fn<tart::mm_fault>,
	.bus_fault = &vector_fn<tart::bus_fault>,
	.usage_fault = &vector_fn<tart::usage_fault>,
	.sv_call = &vector_fn<tart::sv_call>,
	.pend_sv_call = &vector_fn<tart::pend_sv_call>,
	.systick = &vector_fn<tart::systick>,

	.irqs = make_irq_handlers<68>()
};
