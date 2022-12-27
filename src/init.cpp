#include <tart/init.hpp>

#include <tart/main.hpp>

#include <tart/irq_service.hpp>
#include <async/basic.hpp>
#include <tart/log.hpp>
#include <tart/alloc.hpp>

namespace tart {

void init() {
	init_alloc();

	if (user_init)
		user_init();
}

void lib_main() {
	info() << "tart: initialization done, entering async_main" << frg::endlog;

	async::run(::async_main(), irq_s);
}

} // namespace tart
