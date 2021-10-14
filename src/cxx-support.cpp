#include <tart/lib/logger.hpp>
#include <tart/lib/string.hpp>
#include <tart/mem/mem.hpp>
#include <new>

extern "C" [[gnu::used]] void __cxa_pure_virtual() {
	lib::panic("pure virtual function called\r\n");
}

void frg_log(const char *cstring) {
	lib::log("tart: %s\r\n", cstring);
}

void frg_panic(const char *cstring) {
	lib::panic("frigg panic: %s\r\n", cstring);
}

void *operator new(size_t size){
	return tart::alloc().allocate(size);
}

void *operator new[](size_t size){
	return tart::alloc().allocate(size);
}

void operator delete(void *p){
	tart::alloc().free(p);
}

void operator delete[](void *p){
	tart::alloc().free(p);
}

void operator delete(void *p, size_t size){
	tart::alloc().deallocate(p, size);
}

void operator delete[](void *p, size_t size){
	tart::alloc().deallocate(p, size);
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
