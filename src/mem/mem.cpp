
#include <tart/mem/mem.hpp>
#include <tart/lib/logger.hpp>

#include <frg/manual_box.hpp>

#include <tart/chip/memory.hpp>

namespace tart {

uintptr_t bump_policy::map(size_t size, size_t align) {
	top_ = (top_ + align - 1) & ~(align - 1);
	auto ptr = top_;

	top_ += size;

	if (ptr + size > end_) {
		lib::panic("tart: out of memory, tried to allocate %lu bytes with %lu alignment\r\n", size, align);
	}

	memset(reinterpret_cast<void *>(ptr), 0, size);
	return ptr;
}

void bump_policy::unmap(uintptr_t, size_t) {
	lib::panic("tart: bump_policy::unmap is unimplemented\r\n");
}

// --------------------------------------------------------------------

namespace {
	frg::manual_box<bump_policy> policy_;
	frg::manual_box<frg::slab_pool<bump_policy, lib::noop_lock>> pool_;
	frg::manual_box<allocator> alloc_;
}

allocator &alloc() {
	return *alloc_;
}

void init_alloc() {
	auto [start, end] = chip::get_heap_area();

	lib::log("tart: initializing heap 0x%08x-0x%08x\r\n", start, end);

	policy_.initialize(start, end);
	pool_.initialize(*policy_);
	alloc_.initialize(pool_.get());
}

} // namespace tart
