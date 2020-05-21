#include "mem.hpp"
#include <lib/logger.hpp>

extern "C" int _bss_end;

namespace mem {

bump_policy::bump_policy()
:top_((reinterpret_cast<uintptr_t>(&_bss_end) + pagesize - 1)
		& ~(pagesize - 1)) {}

uintptr_t bump_policy::map(size_t s, size_t alignment) {
	uintptr_t sp;
	uintptr_t p = (top_ = (top_ + alignment - 1) & ~(alignment - 1));

	asm volatile ("mov %[v], sp" : [v] "=r" (sp) : : "memory");

	lib::log("bump_policy::map: allocating %lu bytes at 0x%lx\r\n", s, p);
	top_ += s;

	if (p + s > sp) {
		lib::panic("bump_policy::map: agh, the heap is trampling over our stack, bailing out\r\n");
	}

	memset(reinterpret_cast<void *>(p), 0, s);
	return p;
}

void bump_policy::unmap(uintptr_t, size_t) {
	lib::panic("bump_policy::unmap: stub unmap called\r\n");
}

// --------------------------------------------------------------------

namespace {
	bump_policy this_policy_;
	frg::slab_pool<bump_policy, lib::noop_lock> this_pool_{this_policy_};
	frg::slab_allocator this_allocator_{&this_pool_};
}

frg::slab_allocator<bump_policy, lib::noop_lock> &get_allocator() {
	return this_allocator_;
}

} // namespace mem

void *operator new(size_t size){
	return mem::this_allocator_.allocate(size);
}

void *operator new[](size_t size){
	return mem::this_allocator_.allocate(size);
}

void operator delete(void *p){
	mem::this_allocator_.free(p);
}

void operator delete[](void *p){
	mem::this_allocator_.free(p);
}

void operator delete(void *p, size_t size){
	mem::this_allocator_.deallocate(p, size);
}

void operator delete[](void *p, size_t size){
	mem::this_allocator_.deallocate(p, size);
}
