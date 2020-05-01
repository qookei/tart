#include <lib/utils.hpp>
#include <lib/logger.hpp>

namespace async::platform {
	using mutex = lib::noop_lock;

	[[noreturn]] inline void panic(const char *str) {
		lib::log("%s\r\n", str);
		while(1);
		__builtin_unreachable();
	}
}
