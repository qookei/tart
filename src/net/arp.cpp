#include "arp.hpp"
#include <net/builder.hpp>

namespace net {

async::result<void> arp_processor::push_packet(
		mem::buffer &&b, ethernet_frame &&f) {
	auto arp = arp_frame::from_ethernet_frame(f);

	lib::log("arp_processor::push_packet: got arp packet, operation %04x\r\n", arp.operation);
	if (arp.operation == 1) {
		if (arp.target_proto_address == our_ip_) {
			lib::log("arp_processor::push_packet: arp request for our ip!\r\n");
			co_await submit_reply_for(arp.sender_hw_address, arp.sender_proto_address);
		}

		lib::log("net::process_arp: who has %u.%u.%u.%u, tell %u.%u.%u.%u (%02x:%02x:%02x:%02x:%02x:%02x)\r\n",
			arp.target_proto_address[0],
			arp.target_proto_address[1],
			arp.target_proto_address[2],
			arp.target_proto_address[3],
			arp.sender_proto_address[0],
			arp.sender_proto_address[1],
			arp.sender_proto_address[2],
			arp.sender_proto_address[3],
			arp.sender_hw_address[0],
			arp.sender_hw_address[1],
			arp.sender_hw_address[2],
			arp.sender_hw_address[3],
			arp.sender_hw_address[4],
			arp.sender_hw_address[5]);
	} else if (arp.operation == 2) {
		lib::log("net::process_arp: %u.%u.%u.%u is at %02x:%02x:%02x:%02x:%02x:%02x\r\n",
			arp.sender_proto_address[0],
			arp.sender_proto_address[1],
			arp.sender_proto_address[2],
			arp.sender_proto_address[3],
			arp.sender_hw_address[0],
			arp.sender_hw_address[1],
			arp.sender_hw_address[2],
			arp.sender_hw_address[3],
			arp.sender_hw_address[4],
			arp.sender_hw_address[5]);

		for (auto r : routes_) {
			if (r->ip_ != arp.sender_proto_address)
				continue;

			// we know the route has a matching ip or mac
			r->ip_ = arp.sender_proto_address;
			r->mac_ = arp.sender_hw_address;
			r->resolved_ = true;
			r->doorbell_.ring();
		}
	}

	co_return;
}

bool arp_processor::matches(const ethernet_frame &f) {
	return f.type == ethernet_frame::arp_type;
}

async::result<frg::optional<mac>> arp_processor::mac_of(ipv4_addr ip) {
	// look for an existing route first
	for (auto r : routes_) {
		if (r->ip_ != ip)
			continue;

		if (!r->resolved_)
			co_await r->doorbell_.async_wait();

		assert(r->resolved_);
		co_return r->mac_;
	}

	// none found, add and wait for one
	auto r = *routes_.push_back(new route{ip, {}, false, {}, {}});

	co_await submit_query_for(ip);

	if (!r->resolved_)
		co_await r->doorbell_.async_wait();

	assert(r->resolved_);
	co_return r->mac_;
}

async::result<void> arp_processor::submit_query_for(ipv4_addr ip) {
	auto buffer = build_packet(
		ethernet_frame{sender_->mac(), mac_addr::broadcast, ethernet_frame::arp_type, nullptr, 0},
		arp_frame::query_for(sender_->mac(), our_ip_, ip)
	);

	co_await sender_->send_packet(std::move(buffer));
}

async::result<void> arp_processor::submit_reply_for(mac target_mac, ipv4_addr target_ip) {
	auto buffer = build_packet(
		ethernet_frame{sender_->mac(), target_mac, ethernet_frame::arp_type, nullptr, 0},
		arp_frame::reply_for(sender_->mac(), our_ip_, target_mac, target_ip)
	);

	co_await sender_->send_packet(std::move(buffer));
}

}; // namespace net
