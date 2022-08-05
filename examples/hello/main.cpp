#include <tart/main.hpp>
#include <tart/log.hpp>
#include <tart/chip/uart.hpp>
#include <tart/chip/rp2040/hw.hpp>

namespace tart {
	extern tart::chip::uart *log_sink;
}

void user_init() {
	tart::chip::uart0.init(115200);
	tart::log_sink = &tart::chip::uart0;
}

async::result<void> async_main() {
	tart::info() << "info: Hello world!" << frg::endlog;
	tart::dbg() << "debug: Hello world!" << frg::endlog;

	while (1) {
		char buf[32];
		co_await tart::chip::uart0.await_rx();
		size_t n = tart::chip::uart0.recv_sync(buf, 32);

		tart::dbg() << "recv " << frg::string_view{buf, n} << frg::endlog;
	}

	co_return;
}
