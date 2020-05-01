#include "process.hpp"
#include <lib/logger.hpp>

#include <net/ether.hpp>
#include <net/arp.hpp>

namespace net {

namespace {

void process_arp(ethernet_frame &frame) {
	auto arp = arp_frame::from_ethernet_frame(frame);
	lib::log("net::process_arp: got arp packet, hw type %04x, proto type %04x, hw len %02x, proto len %02x, operation %04x\r\n", arp.hw_type, arp.proto_type, arp.hw_len, arp.proto_len, arp.operation);
	if (arp.operation == 1) {
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
	}
}

void process_ipv4(ethernet_frame &frame) {
	auto ipv4 = ipv4_frame::from_ethernet_frame(frame);

	const char *proto_str = "unknown";
	switch (ipv4.protocol) {
		case ipv4_frame::icmp_proto: proto_str = "icmp"; break;
		case ipv4_frame::igmp_proto: proto_str = "igmp"; break;
		case ipv4_frame::tcp_proto: proto_str = "tcp"; break;
		case ipv4_frame::udp_proto: proto_str = "udp"; break;
		case ipv4_frame::encap_proto: proto_str = "encap"; break;
		case ipv4_frame::ospf_proto: proto_str = "ospf"; break;
		case ipv4_frame::sctp_proto: proto_str = "sctp"; break;
	}

	lib::log("net::process_ipv4: got ipv4 packet, source %u.%u.%u.%u, dest %u.%u.%u.%u, protocol %u (%s), ttl %u\r\n",
			ipv4.source[0],
			ipv4.source[1],
			ipv4.source[2],
			ipv4.source[3],
			ipv4.dest[0],
			ipv4.dest[1],
			ipv4.dest[2],ipv4.dest[3],
			ipv4.protocol, proto_str, ipv4.ttl);
}

} // namespace anonymous

void process_packet(void *data, size_t size) {
	auto ether = ethernet_frame::from_bytes(data, size);
	if (ether.type <= 1500) {
		lib::log("net::process_packet: no ethertype value, dropping\r\n");
		return;
	}

	lib::log("net::process_packet: got ethernet frame, from %02x:%02x:%02x:%02x:%02x:%02x, to %02x:%02x:%02x:%02x:%02x:%02x, type %04x\r\n",
			ether.source[0], ether.source[1], ether.source[2],
			ether.source[3], ether.source[4], ether.source[5],
			ether.dest[0], ether.dest[1], ether.dest[2],
			ether.dest[3], ether.dest[4], ether.dest[5],
			ether.type);

	switch (ether.type) {
		case ethernet_frame::arp_type:
			process_arp(ether);
			break;
		case ethernet_frame::ipv4_type:
			process_ipv4(ether);
			break;
	}
}

} // namespace net
