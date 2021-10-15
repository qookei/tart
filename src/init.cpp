#include <tart/init.hpp>

#include <tart/chip/init.hpp>
#include <tart/arch/init.hpp>
#include <tart/main.hpp>

#include <tart/irq_service.hpp>
#include <async/basic.hpp>
#include <tart/lib/logger.hpp>
#include <tart/mem/mem.hpp>

namespace tart {

void early_init() {
	chip::early_init();
	lib::log("tart: early chip init done\r\n");
	arch::early_init();
	lib::log("tart: early arch init done\r\n");
	init_alloc();
}

void late_init() {
}

void lib_main() {
	async::run_queue rq;
	async::queue_scope qs{&rq};
	irq_service ios{&rq};

	lib::log("tart: hello!\r\n");

	lib::log("tart: entering async_main\r\n");
	async::run(::async_main(), rq.run_token(), ios);
}

} // namespace tart
