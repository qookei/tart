#include <periph/gpio.hpp>
#include <periph/spi.hpp>
#include <drivers/enc28j60.hpp>
#include <lib/logger.hpp>
#include <async/basic.hpp>
#include <net/process.hpp>

namespace service {

void run() {
	async::run_queue rq;
	async::queue_scope qs{&rq};

	struct dummy_io_service { void wait() { } } ios;

	lib::log("tart: hello!\r\n");

	spi::spi_dev dev2{spi::get_spi(2), gpio::pa2};
	drivers::enc28j60_nic nic{&dev2};
	net::processor pr;

	// 57:95:77:9C:48:BA
	nic.run({0x57, 0x95, 0x77, 0x9C, 0x48, 0xBA}, pr);
	pr.process_packets();

	lib::log("tart: entering async run_queue\r\n");
	async::run_forever(rq.run_token(), ios);
}

} // namespace service
