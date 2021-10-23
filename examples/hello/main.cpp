#include <tart/main.hpp>
#include <tart/log.hpp>

async::result<void> async_main() {
	tart::info() << "Hello world!\r\n" << frg::endlog;
	co_return;
}
