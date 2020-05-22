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

			assert(f.hw_type == 0x0001);
			assert(f.proto_type == 0x0800);
			assert(f.hw_len == 6);
			assert(f.proto_len == 4);

			f.sender_hw_address = mac::from_bytes(bytes + 8);
			f.target_hw_address = mac::from_bytes(bytes + 18);

			f.sender_proto_address = ipv4_addr::from_bytes(bytes + 14);
			f.target_proto_address = ipv4_addr::from_bytes(bytes + 24);
			return f;
		}

		size_t get_size() {
			return 8 + hw_len * 2 + proto_len * 2;
		}

		void *to_bytes(void *ptr) {
			uint8_t *dest = static_cast<uint8_t *>(ptr);
			*dest++ = (hw_type >> 8) & 0xFF;
			*dest++ = (hw_type) & 0xFF;
			*dest++ = (proto_type >> 8) & 0xFF;
			*dest++ = (proto_type) & 0xFF;
			*dest++ = hw_len;
			*dest++ = proto_len;

			assert(hw_type == 0x0001);
			assert(proto_type == 0x0800);
			assert(hw_len == 6);
			assert(proto_len == 4);

			*dest++ = (operation >> 8) & 0xFF;
			*dest++ = (operation) & 0xFF;

			dest = static_cast<uint8_t *>(sender_hw_address.to_bytes(dest));
			dest = static_cast<uint8_t *>(sender_proto_address.to_bytes(dest));
			dest = static_cast<uint8_t *>(target_hw_address.to_bytes(dest));
			dest = static_cast<uint8_t *>(target_proto_address.to_bytes(dest));

			return dest;
		}
	};
} // namespace net
