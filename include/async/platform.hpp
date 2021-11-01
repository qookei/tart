#include <frg/spinlock.hpp>
#include <assert.h>

extern "C" void frg_panic(const char *str);

namespace async::platform {
	using mutex = frg::ticket_spinlock;

	[[noreturn]] inline void panic(const char *str) {

		frg_panic(str);
		__builtin_unreachable();
	}
} // namespace async::platform
