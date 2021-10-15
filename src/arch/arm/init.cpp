#include <tart/init.hpp>
#include <tart/arch/init.hpp>

#include <tart/arch/arm/systick.hpp>

#include <tart/lib/string.hpp>
#include <tart/lib/logger.hpp>

#include <stdint.h>

extern "C" void (*__init_array_start)(), (*__init_array_end)();
extern "C" char __data_start[], __data_end[], __data_loadaddr[];
extern "C" char __bss_start[], __bss_end[];

namespace tart::arch {

extern "C" void reset_handler() {
	memcpy(&__data_start, &__data_loadaddr,
			reinterpret_cast<uintptr_t>(&__data_end)
			- reinterpret_cast<uintptr_t>(&__data_start));
	memset(&__bss_start, 0,
			reinterpret_cast<uintptr_t>(&__bss_end)
			- reinterpret_cast<uintptr_t>(&__bss_start));

	tart::early_init();

	for (auto ctor = &__init_array_start; ctor < &__init_array_end; ctor++) {
		(*ctor)();
	}

	lib::log("tart: %lu global constructor(s) ran\r\n", &__init_array_end - &__init_array_start);

	late_init();
	lib_main();

	while(true);
}

void early_init() {
	systick_init(1000);
}

} // namespace tart::arch
