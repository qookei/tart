#include <periph/gpio.hpp>
#include <periph/spi.hpp>
#include <drivers/enc28j60.hpp>
#include <lib/logger.hpp>
#include <async/basic.hpp>
#include <async/service.hpp>
#include <net/process.hpp>

namespace service {

void run() {
	async::run_queue rq;
	async::queue_scope qs{&rq};
	io_service ios{&rq};

	lib::log("tart: hello!\r\n");

	spi::spi_dev dev2{spi::get_spi(2), gpio::pa2};
	drivers::enc28j60_nic nic{&dev2};
	//net::processor pr;
	net::ether_dispatcher<
		drivers::enc28j60_nic,
		net::null_processor
	> ed{nic};

	// 56:95:77:9C:48:BA
	nic.setup({0x56, 0x95, 0x77, 0x9C, 0x48, 0xBA});
	ed.run();

	ios.attach_waiter(nic);

	lib::log("tart: entering async run_queue\r\n");
	async::run_forever(rq.run_token(), ios);
}

} // namespace service
