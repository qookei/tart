#include <tart/log.hpp>
#include <string.h>
#include <tart/alloc.hpp>
#include <new>

extern "C" [[gnu::used]] void __cxa_pure_virtual() {
	tart::fatal() << "tart: panic: pure virtual function called" << frg::endlog;
	__builtin_unreachable();
}

void frg_log(const char *msg) {
	tart::info() << "tart: " << msg << frg::endlog;
}

void frg_panic(const char *msg) {
	tart::fatal() << "tart: panic: " << msg << frg::endlog;
	__builtin_unreachable();
}

[[noreturn]] void assert_failure__(const char *expr, const char *file, int line, const char *func) {
	tart::fatal() << "tart: " << file << ":" << line << ": " << func << ": assertion '" << expr << "' failed" << frg::endlog;
	__builtin_unreachable();
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
