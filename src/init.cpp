#include <tart/init.hpp>

#include <tart/chip/init.hpp>
#include <tart/main.hpp>

#include <tart/async/service.hpp>
#include <async/basic.hpp>
#include <tart/lib/logger.hpp>

namespace tart {

void early_init() {
	chip::early_init();
}

void late_init() {
}

void lib_main() {
	async::run_queue rq;
	async::queue_scope qs{&rq};
	service::io_service ios{&rq};

	lib::log("tart: hello!\r\n");

	lib::log("tart: entering async_main\r\n");
	async::run(::async_main(), rq.run_token(), ios);
}

} // namespace tart


// TODO: remove me
async::result<void> async_main() {
	lib::log("tart: hi!\r\n");
	lib::panic("test");
	co_return;
}
