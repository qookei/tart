#include <lib/logger.hpp>
#include <lib/string.hpp>

extern "C" [[gnu::used]] void __cxa_pure_virtual() {
	lib::panic("pure virtual function called\r\n");
}

#ifdef CLANG_MEMES
extern "C" {
	[[gnu::used]] void *__aeabi_memclr(void *dest, size_t size) {
		return memset(dest, 0, size);
	}

	[[gnu::used]] void *__aeabi_memclr4(void *dest, size_t size) {
		return memset(dest, 0, size);
	}

	[[gnu::used]] void *__aeabi_memclr8(void *dest, size_t size) {
		return memset(dest, 0, size);
	}

	[[gnu::used]] void *__aeabi_memcpy(void *dest, const void *src, size_t size) {
		return memcpy(dest, src, size);
	}

	[[gnu::used]] void *__aeabi_memcpy4(void *dest, const void *src, size_t size) {
		return memcpy(dest, src, size);
	}

}
#endif
