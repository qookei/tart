#include <tart/arch/platform.hpp>

#include <tart/lib/string.hpp>

#include <stdint.h>
#include <stddef.h>

struct vector_table {
	using fn = void (*)();

	void *stack;
	fn reset;
	fn nmi;
	fn hard_fault;
	fn mm_fault;
	fn bus_fault;
	fn usage_fault;
	uintptr_t reserved1[4];
	fn sv_call;
	uintptr_t reserved2;
	fn pend_sv_call;
	fn systick;

	// TODO: get irq count and handler pointers from platform
	fn irq[68];
};

extern "C" char core0_stack[];

[[gnu::section(".vectors"), gnu::used]]
vector_table vec_ = {
	.stack = core0_stack,
	.reset = platform::entry,
	.nmi = platform::nmi,
	.hard_fault = platform::hard_fault,
	.mm_fault = platform::mm_fault,
	.bus_fault = platform::bus_fault,
	.usage_fault = platform::usage_fault,
	.sv_call = platform::sv_call,
	.pend_sv_call = platform::pend_sv_call,
	.systick = platform::systick,
};

extern "C" void (*__init_array_start)(), (*__init_array_end)();
extern "C" uint32_t __data_start, __data_end, __data_loadaddr;
extern "C" uint32_t __bss_start, __bss_end;

extern "C" void reset_handler() {
	platform::entry();
}

namespace service {
	void run();
} // namespace service

void platform::entry() {
	memcpy(&__data_start, &__data_loadaddr, static_cast<size_t>(&__data_end - &__data_start));
	memset(&__bss_start, 0, static_cast<size_t>(&__bss_end - &__bss_start));

	for (auto ctor = &__init_array_start; ctor < &__init_array_end; ctor++) {
		(*ctor)();
	}

	platform::setup();
	service::run();

	while(true);
}
