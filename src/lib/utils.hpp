#pragma once

namespace lib {

struct noop_lock {
	void lock() { }
	bool try_lock() { return true; }
	void unlock() { }
};

} // namespace lib
