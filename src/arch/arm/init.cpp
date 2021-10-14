#include <tart/init.hpp>
#include <tart/arch/init.hpp>

#include <tart/lib/string.hpp>

#include <stdint.h>
#include <stddef.h>

extern "C" void (*__init_array_start)(), (*__init_array_end)();
extern "C" char __data_start[], __data_end[], __data_loadaddr[];
extern "C" char __bss_start[], __bss_end[];

namespace tart {

extern "C" void reset_handler() {
	memcpy(&__data_start, &__data_loadaddr,
			reinterpret_cast<uintptr_t>(&__data_end)
			- reinterpret_cast<uintptr_t>(&__data_start));
	memset(&__bss_start, 0,
			reinterpret_cast<uintptr_t>(&__bss_end)
			- reinterpret_cast<uintptr_t>(&__bss_start));

	early_init();

	for (auto ctor = &__init_array_start; ctor < &__init_array_end; ctor++) {
		(*ctor)();
	}

	late_init();
	lib_main();

	while(true);
}

} // namespace tart
