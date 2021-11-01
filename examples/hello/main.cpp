#include <tart/main.hpp>
#include <tart/log.hpp>
#include <tart/chip/uart.hpp>

namespace tart {
	extern tart::chip::uart *log_sink;
}

void user_init() {
	tart::chip::uart0.init(115200);
	tart::log_sink = &tart::chip::uart0;
}

async::result<void> async_main() {
	tart::info() << "Hello world!\r\n" << frg::endlog;
	co_return;
}
