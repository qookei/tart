#include <tart/chip/memory.hpp>

extern "C" char __image_ram_end[];

namespace tart::chip {

frg::tuple<uintptr_t, uintptr_t> get_heap_area() {
	return {reinterpret_cast<uintptr_t>(__image_ram_end), 0x20040000};
}

} // namespace tart::chip
