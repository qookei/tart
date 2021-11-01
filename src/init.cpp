#include <tart/init.hpp>

#include <tart/chip/init.hpp>
#include <tart/arch/init.hpp>
#include <tart/main.hpp>

#include <tart/irq_service.hpp>
#include <async/basic.hpp>
#include <tart/log.hpp>
#include <tart/mem/mem.hpp>

namespace tart {

void init() {
	chip::init();
	arch::init();
	init_alloc();

	if (user_init)
		user_init();
}

void lib_main() {
	irq_service ios;

	info() << "tart: initialization done, entering async_main" << frg::endlog;

	async::run(::async_main(), ios);
}

} // namespace tart
