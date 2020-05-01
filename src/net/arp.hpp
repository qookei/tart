#pragma once

#include <net/mac.hpp>
#include <net/ipv4.hpp>
#include <net/ether.hpp>

namespace net {
	struct arp_frame {
		uint16_t hw_type;
		uint16_t proto_type;
		uint8_t hw_len;
		uint8_t proto_len;
		uint16_t operation;

		// ?
		mac sender_hw_address;
		ipv4_addr sender_proto_address;

		mac target_hw_address;
		ipv4_addr target_proto_address;

		static arp_frame from_ethernet_frame(const ethernet_frame &data) {
			const uint8_t *bytes = static_cast<const uint8_t *>(data.payload);
			arp_frame f{};
			f.hw_type = (uint16_t(bytes[0]) << 8) | bytes[1];
			f.proto_type = (uint16_t(bytes[2]) << 8) | bytes[3];
			f.hw_len = bytes[4];
			f.proto_len = bytes[5];
			f.operation = (uint16_t(bytes[6]) << 8) | bytes[7];
			f.sender_hw_address = mac::from_bytes(bytes + 8);
			f.target_hw_address = mac::from_bytes(bytes + 18);

			f.sender_proto_address = ipv4_addr::from_bytes(bytes + 14);
			f.target_proto_address = ipv4_addr::from_bytes(bytes + 24);
			return f;
		}
	};
} // namespace net
