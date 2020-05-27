#include <periph/gpio.hpp>
#include <periph/spi.hpp>
#include <drivers/enc28j60.hpp>
#include <lib/logger.hpp>
#include <async/basic.hpp>
#include <async/service.hpp>
#include <net/process.hpp>
#include <net/arp.hpp>

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
