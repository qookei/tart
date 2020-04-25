#include "mem.hpp"
#include <lib/spinlock.hpp>
#include <lib/logger.hpp>

extern "C" int _bss_end;

namespace mem {

namespace {
	bump_policy _this_policy;
	frg::slab_pool<bump_policy, lib::spinlock> _this_allocator{_this_policy};
}

bump_policy::bump_policy()
:_top((reinterpret_cast<uintptr_t>(&_bss_end) + pagesize - 1)
		& ~(pagesize - 1)) {}

uintptr_t bump_policy::map(size_t s) {
	uintptr_t sp;
	uintptr_t p = _top;

	asm volatile ("mov %[v], sp" : [v] "=r" (sp) : : "memory");

	_top += s;

	if (p > sp) {
		// TODO: replace with actual panic
		lib::log("bump_policy::map: agh, the heap is trampling over our stack, bailing out\r\n");
		while(1);
	}

	memset(reinterpret_cast<void *>(p), 0, s);
	return p;
}

void bump_policy::unmap(uintptr_t, size_t) {
	lib::log("bump_policy::unmap: stub unmap called\r\n");
}

void test() {
	lib::log("mem::test: trying to allocate 32 bytes\r\n");
	void *mem1 = _this_allocator.allocate(32);
	lib::log("mem::test: allocated at %p\r\n", mem1);
	lib::log("mem::test: trying to allocate 16 bytes\r\n");
	void *mem2 = _this_allocator.allocate(16);
	lib::log("mem::test: allocated at %p\r\n", mem2);
	lib::log("mem::test: trying to allocate 512 bytes\r\n");
	void *mem3 = _this_allocator.allocate(512);
	lib::log("mem::test: allocated at %p\r\n", mem3);
	_this_allocator.free(mem1);
	lib::log("mem::test: freed 32 bytes\r\n");
	_this_allocator.free(mem2);
	lib::log("mem::test: freed 16 bytes\r\n");
	_this_allocator.free(mem3);
	lib::log("mem::test: freed 512 bytes\r\n");
	lib::log("mem::test: we didn't crash!\r\n");
}

} // namespace mem
