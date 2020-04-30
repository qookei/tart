#pragma once

#include <new>
#include <frg/slab.hpp>
#include <lib/spinlock.hpp>

namespace mem {

struct bump_policy {
	static constexpr size_t slabsize = 2048;
	static constexpr size_t pagesize = 512;
	static constexpr size_t num_buckets = 8;

	bump_policy();

	uintptr_t map(size_t s);
	void unmap(uintptr_t, size_t);

private:
	uintptr_t _top;
};

frg::slab_allocator<bump_policy, lib::spinlock> &get_allocator();

} // namespace mem
