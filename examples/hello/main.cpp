#include <tart/main.hpp>
#include <tart/log.hpp>
#include <tart/chip/uart.hpp>

namespace tart {
	extern tart::chip::uart *log_sink;
}

async::result<void> async_main() {
	tart::log_sink = &tart::chip::uart0;
	tart::info() << "Hello world!\r\n" << frg::endlog;
	co_return;
}
