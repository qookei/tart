#include "arp.hpp"
#include <net/builder.hpp>

namespace net {

async::result<void> arp_processor::push_packet(
		mem::buffer &&b, ethernet_frame &&f) {
	auto arp = arp_frame::from_ethernet_frame(f);

	if (arp.operation == 1) {
		if (arp.target_ip == our_ip_) {
			co_await submit_reply_for(arp.sender_mac, arp.sender_ip);
		}
	} else if (arp.operation == 2) {
		for (auto r : routes_) {
			if (r->ip_ != arp.sender_ip)
				continue;

			r->ip_ = arp.sender_ip;
			r->mac_ = arp.sender_mac;
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
