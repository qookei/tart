#include <tart/lib/utils.hpp>
#include <tart/log.hpp>

#define LIBASYNC_THREAD_LOCAL

namespace async::platform {
	using mutex = lib::noop_lock;

	[[noreturn]] inline void panic(const char *str) {
		tart::info() << str << "\r\n";
		while(1);
		__builtin_unreachable();
	}
} // namespace async::platform
