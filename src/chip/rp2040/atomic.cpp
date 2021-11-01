#include <stdint.h>

extern "C" [[gnu::used]] unsigned int __atomic_fetch_add_4(volatile void *ptr, unsigned int value, int) {
	const auto iptr = static_cast<volatile uint32_t *>(ptr);

	asm ("dmb");
	auto v = *iptr;
	*iptr = *iptr + value;
	asm ("dmb");
	return v;
}

extern "C" [[gnu::used]] unsigned int __atomic_exchange_4(volatile void *ptr, unsigned int value, int) {
	const auto iptr = static_cast<volatile uint32_t *>(ptr);

	asm ("dmb");
	auto v = *iptr;
	*iptr = value;
	asm ("dmb");
	return v;
}
