#pragma once
#include <lib/logger.hpp>

namespace lib {

struct noop_lock {
	void lock() { }
	bool try_lock() { return true; }
	void unlock() { }
};

} // namespace lib

#define _STR(x) #x
#define STR(x) _STR(x)

#define assert(expr) \
	do { \
		if(!(expr)) \
			assert_failure__(STR(expr), __FILE__, __LINE__, __func__);\
	} while(0);

// public ns
[[noreturn]] static inline void assert_failure__(
		const char *expr, const char *file, int line, const char *func) {
	lib::panic("tart: %s:%d: %s: assertion '%s' failed\r\n", file, line, func, expr);
}
