#include "platform.hpp"

#include <lib/string.hpp>

#include <stdint.h>
#include <stddef.h>

struct vector_table {
	void *stack;
	void (*reset)();
	void (*nmi)();
	void (*hard_fault)();
	void (*mm_fault)();
	void (*bus_fault)();
	void (*usage_fault)();
	uintptr_t reserved1[4];
	void (*sv_call)();
	uintptr_t reserved2;
	void (*pend_sv_call)();
	void (*systick)();

	// TODO: get irq count and handler pointers from platform
	void (*irq[68]);
};

extern "C" uint32_t _stack;

[[gnu::section(".vectors")]]
vector_table _vec = {
	.stack = &_stack,
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
extern "C" uint32_t _data_start, _data_end, _data_loadaddr;
extern "C" uint32_t _bss_start, _bss_end;

extern "C" void reset_handler() {
	platform::entry();
}

void platform::entry() {
	memcpy(&_data_start, &_data_loadaddr, static_cast<size_t>(&_data_end - &_data_start));
	memset(&_bss_start, 0, static_cast<size_t>(&_bss_end - &_bss_start));

	for (auto ctor = &__init_array_start; ctor < &__init_array_end; ctor++) {
		(*ctor)();
	}

	platform::setup();
	platform::run();

	while(true);
}
