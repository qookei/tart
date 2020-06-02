#include "icmp.hpp"
#include <net/builder.hpp>

namespace net {

async::result<void> icmp_processor::push_packet(mem::buffer &&b, ipv4_frame &&f) {
	icmp_frame icmp = icmp_frame::from_ipv4_frame(f);

	if (icmp.type == icmp_frame::echo_request_type) {
		// silently drop pings not targetted at us
		if (f.dest != sender_->ip())
			co_return;

		lib::log("icmp_processor::push_packet: icmp echo request received\n");
		auto resp = build_packet(
			ethernet_frame{sender_->mac(), f.ether.source, ethernet_frame::ipv4_type, nullptr, 0},
			ipv4_frame{sender_->ip(), f.source, ipv4_frame::icmp_proto, 64, 0, 5, nullptr, 0, {}},
			icmp_frame{icmp_frame::echo_reply_type, 0, 0, icmp.ident, icmp.seq, icmp.payload, icmp.payload_size}
		);

		resp.dump();

		co_await sender_->send_packet(std::move(resp));
	}

	co_return;
}

bool icmp_processor::matches(const ipv4_frame &f) {
	return f.protocol == ipv4_frame::icmp_proto;
}

} // namespace net
