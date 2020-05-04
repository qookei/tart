#include <lib/logger.hpp>

extern "C" [[gnu::used]] void __cxa_pure_virtual() {
	lib::panic("pure virtual function called\r\n");
}
