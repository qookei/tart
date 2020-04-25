#pragma once

#include <new>
#include <frg/slab.hpp>

namespace mem {

struct bump_policy {
	static constexpr size_t slabsize = 1280;
	static constexpr size_t pagesize = 256;
	static constexpr size_t num_buckets = 8;

	bump_policy();

	uintptr_t map(size_t s);
	void unmap(uintptr_t, size_t);

private:
	uintptr_t _top;
};


} // namespace mem
