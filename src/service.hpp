#pragma once

#include <drivers/enc28j60.hpp>
#include <net/ipv4/icmp.hpp>
#include <async/service.hpp>
#include <net/ipv4/udp.hpp>
#include <net/ipv4/tcp.hpp>
#include <net/dispatch.hpp>
#include <async/basic.hpp>
#include <lib/logger.hpp>
#include <net/arp.hpp>

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
		ed_.set_ip({192, 168, 1, 69});
		ed_.run();

		/*
		auto &udp_proc = ed_.nth_processor<0>().nth_processor<1>();
		udp_proc.set_arp_processor(&ed_.nth_processor<1>());

		auto sock = co_await udp_proc.listen(1337);

		lib::log("tart: %u.%u.%u.%u connected (ports: in %u, out %u)\r\n",
				sock->ip()[0], sock->ip()[1],
				sock->ip()[2], sock->ip()[3],
				sock->in_port(), sock->out_port());
		while (true) {
			auto b = co_await sock->recv();
			lib::log("tart: received '%.*s'\r\n", b.size() - 1, b.data());
			co_await sock->send(b.data(), b.size());
		}*/

		auto &tcp_proc = ed_.nth_processor<0>().nth_processor<2>();
		tcp_proc.set_arp_processor(&ed_.nth_processor<1>());

		auto sock = co_await tcp_proc.listen(1337);

		lib::log("tart: %u.%u.%u.%u connected (ports: in %u, out %u)\r\n",
				sock->ip()[0], sock->ip()[1],
				sock->ip()[2], sock->ip()[3],
				sock->in_port(), sock->out_port());
		//while (true) {
		auto b = co_await sock->recv();
		lib::log("tart: received '%.*s'\r\n", b.size() - 1, b.data());
		co_await sock->send(b.data(), b.size());
		co_await sock->close();
		//}
	}

private:
	io_service *ios_;

	spi::spi_dev dev_;
	drivers::enc28j60_nic nic_;
	net::ether_dispatcher<
		drivers::enc28j60_nic,
		net::ipv4_processor<
			net::icmp_processor,
			net::udp_processor,
			net::tcp_processor
		>,
		net::arp_processor
	> ed_;
};

} // namespace service
