#include <stdint.h>

extern "C" [[gnu::used]] unsigned int __atomic_exchange_4(volatile void *ptr, unsigned int value, int) {
	asm ("dmb");
	auto v = *static_cast<volatile uint32_t *>(ptr);
	*static_cast<volatile uint32_t *>(ptr) = value;
	asm ("dmb");
	return v;
}
