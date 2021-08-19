#pragma once

//#include <drivers/enc28j60.hpp>
//#include <net/ipv4/icmp.hpp>
#include <async/service.hpp>
//#include <net/ipv4/udp.hpp>
//#include <net/ipv4/tcp.hpp>
//#include <net/dispatch.hpp>
#include <async/basic.hpp>
//#include <frg/string.hpp>
//#include <lib/logger.hpp>
//#include <net/arp.hpp>

namespace service {

struct service {
	service(io_service *ios)
	: ios_{ios} { } /*, dev_{spi::get_spi(2), gpio::pa2},
			nic_{&dev_}, ed_{nic_} {
		ios_->attach_waiter(nic_);
	}*/

	async::detached async_main() { 
		// 56:95:77:9C:48:BA
//		nic_.setup({0x56, 0x95, 0x77, 0x9C, 0x48, 0xBA});
//		ed_.set_ip({192, 168, 1, 69});
//		ed_.run();

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

/*		auto &tcp_proc = ed_.nth_processor<0>().nth_processor<2>();
		tcp_proc.set_arp_processor(&ed_.nth_processor<1>());

		while (true) {
			auto sock = co_await tcp_proc.listen(80);
			connection_worker(sock);
		}*/

		co_return;
	}

private:
	io_service *ios_;

//	spi::spi_dev dev_;
//	drivers::enc28j60_nic nic_;
//	net::ether_dispatcher<
//		drivers::enc28j60_nic,
//		net::ipv4_processor<
//			net::icmp_processor,
//			net::udp_processor,
//			net::tcp_processor
//		>,
//		net::arp_processor
//	> ed_;

/*	async::detached connection_worker(auto sock) {
		lib::log("tart: %u.%u.%u.%u connected (ports: in %u, out %u)\r\n",
				sock->ip()[0], sock->ip()[1],
				sock->ip()[2], sock->ip()[3],
				sock->in_port(), sock->out_port());

		while (sock->connected()) {
			auto packet = co_await sock->recv();
			if (!packet)
				continue;

			auto b = std::move(*packet);

			frg::string_view req{static_cast<char *>(b.data()), b.size()};
			size_t newl = req.find_first('\n');
			auto http_req = req.sub_string(0, newl);

			auto s1 = http_req.find_first(' ');
			auto s2 = http_req.find_last(' ');

			if (s1 == size_t(-1) || s2 == size_t(-1))
				continue;

			auto type = http_req.sub_string(0, s1);
			auto path = http_req.sub_string(s1 + 1, s2 - s1 - 1);

			lib::log("tart: got a http request!, type \"%.*s\", path \"%.*s\"\r\n", type.size(), type.data(), path.size(), path.data());

			frg::string_view resp;
			if (type != "GET")
				resp = "HTTP/1.1 400 Bad request\nServer: tart\nContent-Type: text/html\nContent-Length: 20\n\nBad request ...what?";
			else if (path == "/" || path == "/index.html")
				resp = "HTTP/1.1 200 OK\nServer: tart\nContent-Type: text/html\nContent-Length: 101\n\nHello, world! This web page was served to you from <a href=\"https://github.com/qookei/tart\">tart</a>!";
			else if (path == "/other.html")
				resp = "HTTP/1.1 200 OK\nServer: tart\nContent-Type: text/html\nContent-Length: 80\n\nHello, world ...again! This is a different page. <a href=\"index.html\">Go back</a>";
			else
				resp = "HTTP/1.1 404 File not found\nServer: tart\nContent-Type: text/html\nContent-Length: 38\n\nI don't know what you're asking for...";

			co_await sock->send(resp.data(), resp.size());
		}

		lib::log("tart: %u.%u.%u.%u:%u disconnected\r\n",
				sock->ip()[0], sock->ip()[1],
				sock->ip()[2], sock->ip()[3],
				sock->out_port());

		co_await sock->close();
	}*/
};

} // namespace service
