#pragma once

#include <async/service.hpp>

namespace service {

struct service {
	service(io_service *ios)
	: ios_{ios}, dev_{spi::get_spi(2), gpio::pa2},
			nic_{&dev_}, ed_{nic_} {
		ios_->attach_waiter(nic_);
	}

	async::detached async_main() { 
		// 56:95:77:9C:48:BA
		nic_.setup({0x56, 0x95, 0x77, 0x9C, 0x48, 0xBA});
		ed_.run();

		ed_.template nth_processor<1>().set_our_ip({192, 168, 1, 69});

		auto mac = *co_await ed_.template nth_processor<1>().mac_of({192, 168, 1, 102});
		lib::log("tart: 192.168.1.102 is at %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

private:
	io_service *ios_;

	spi::spi_dev dev_;
	drivers::enc28j60_nic nic_;
	net::ether_dispatcher<
		drivers::enc28j60_nic,
		net::ipv4_processor<>,
		net::arp_processor
	> ed_;
};

} // namespace service
