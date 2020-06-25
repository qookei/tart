#include "arp.hpp"
#include <net/builder.hpp>

namespace net {

async::result<void> arp_processor::push_packet(
		mem::buffer &&b, ethernet_frame &&f) {
	auto arp = arp_frame::from_ethernet_frame(f);

	if (arp.operation == 1) {
		if (arp.target_ip == sender_->ip()) {
			co_await submit_reply_for(arp.sender_mac, arp.sender_ip);
		}
	} else if (arp.operation == 2) {
		for (auto r : routes_) {
			if (r->ip_ != arp.sender_ip)
				continue;

			r->ip_ = arp.sender_ip;
			r->mac_ = arp.sender_mac;
			r->resolved_ = true;
			r->doorbell_.set_value();
		}
	}

	co_return;
}

bool arp_processor::matches(const ethernet_frame &f) {
	return f.type == ethernet_frame::arp_type;
}

async::result<mac_addr> arp_processor::mac_of(ipv4_addr ip) {
	// look for an existing route first
	for (auto r : routes_) {
		if (r->ip_ != ip)
			continue;

		co_await r->doorbell_.async_get();

		assert(r->resolved_);
		co_return r->mac_;
	}

	// none found, add and wait for one
	auto r = *routes_.push_back(new route{ip, {}, false, {}, {}});

	co_await submit_query_for(ip);
	co_await r->doorbell_.async_get();

	assert(r->resolved_);
	co_return r->mac_;
}

async::result<void> arp_processor::submit_query_for(ipv4_addr ip) {
	auto buffer = build_packet(
		ethernet_frame{sender_->mac(), mac::broadcast, ethernet_frame::arp_type, nullptr, 0},
		arp_frame::query_for(sender_->mac(), sender_->ip(), ip)
	);

	co_await sender_->send_packet(std::move(buffer));
}

async::result<void> arp_processor::submit_reply_for(mac_addr target_mac, ipv4_addr target_ip) {
	auto buffer = build_packet(
		ethernet_frame{sender_->mac(), target_mac, ethernet_frame::arp_type, nullptr, 0},
		arp_frame::reply_for(sender_->mac(), sender_->ip(), target_mac, target_ip)
	);

	co_await sender_->send_packet(std::move(buffer));
}

}; // namespace net
