#pragma once

#include <frg/spinlock.hpp>
#include <frg/slab.hpp>
#include <tart/lib/utils.hpp>

namespace tart {

struct bump_policy {
	static constexpr size_t sb_size = 2048;
	static constexpr size_t slabsize = 2048;
	static constexpr size_t pagesize = 512;
	static constexpr size_t num_buckets = 8;

	bump_policy(uintptr_t start, uintptr_t end)
	: top_{start}, end_{end} { }

	uintptr_t map(size_t size, size_t align);
	void unmap(uintptr_t, size_t);

private:
	uintptr_t top_;
	uintptr_t end_;
};

using allocator = frg::slab_allocator<bump_policy, frg::ticket_spinlock>;

allocator &alloc();

void init_alloc();

} // namespace tart
