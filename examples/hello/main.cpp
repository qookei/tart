#include <tart/main.hpp>
#include <tart/lib/logger.hpp>

async::result<void> async_main() {
	lib::log("Hello world!\r\n");
	co_return;
}
