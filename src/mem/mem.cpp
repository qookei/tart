
#include <tart/mem/mem.hpp>
#include <tart/log.hpp>
#include <tart/log.hpp>

#include <frg/manual_box.hpp>

#include <tart/chip/memory.hpp>

namespace tart {

uintptr_t bump_policy::map(size_t size, size_t align) {
	top_ = (top_ + align - 1) & ~(align - 1);
	auto ptr = top_;

	top_ += size;

	if (ptr + size > end_) {
		fatal() << "tart: out of memory, tried to allocate " << size << " bytes with "
			<< align << " alignment" << frg::endlog;
	}

	memset(reinterpret_cast<void *>(ptr), 0, size);
	return ptr;
}

void bump_policy::unmap(uintptr_t, size_t) {
	fatal() << "tart: bump_policy::unmap is unimplemented" << frg::endlog;
}

// --------------------------------------------------------------------

namespace {
	frg::manual_box<bump_policy> policy_;
	frg::manual_box<frg::slab_pool<bump_policy, frg::ticket_spinlock>> pool_;
	frg::manual_box<allocator> alloc_;
}

allocator &alloc() {
	return *alloc_;
}

void init_alloc() {
	auto [start, end] = chip::get_heap_area();

	info() << "tart: initializing heap 0x" << frg::hex_fmt{start}
		<< "-0x" << frg::hex_fmt{end} << frg::endlog;
	policy_.initialize(start, end);
	pool_.initialize(*policy_);
	alloc_.initialize(pool_.get());
}

} // namespace tart
