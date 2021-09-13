#include <tart/async/service.hpp>
#include <async/basic.hpp>
#include <tart/lib/logger.hpp>
#include <service.hpp>

namespace service {

void run() {
	async::run_queue rq;
	async::queue_scope qs{&rq};
	io_service ios{&rq};
	service s{&ios};

	lib::log("tart: hello!\r\n");

	s.async_main();

	lib::log("tart: entering async run_queue\r\n");
	async::run_forever(rq.run_token(), ios);
}

} // namespace service
