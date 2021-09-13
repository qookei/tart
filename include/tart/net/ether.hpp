#pragma once

#include <stdint.h>
#include <tart/net/address.hpp>

namespace net {
	struct ethernet_frame {
		mac_addr source;
		mac_addr dest;

		uint16_t type;

		void *payload;
		size_t payload_size;

		static ethernet_frame from_bytes(void *data, size_t size) {
			uint8_t *bytes = static_cast<uint8_t *>(data);
			auto dst = mac_addr::from_bytes(bytes);
			auto src = mac_addr::from_bytes(bytes + 6);

			uint16_t type = (uint16_t(bytes[12]) << 8) | bytes[13];
			void *payload = bytes + 14;

			return ethernet_frame{src, dst, type, payload, size - 18};
		}

		// This does not include any padding or the CRC
		size_t get_size() {
			return 14;
		}

		void *to_bytes(void *ptr) {
			uint8_t *d = static_cast<uint8_t *>(ptr);
			d = static_cast<uint8_t *>(dest.to_bytes(d));
			d = static_cast<uint8_t *>(source.to_bytes(d));
			*d++ = (type >> 8) & 0xFF;
			*d++ = (type) & 0xFF;

			return d;
		}

		static constexpr uint16_t ipv4_type = 0x0800;
		static constexpr uint16_t ipv6_type = 0x86DD;
		static constexpr uint16_t arp_type = 0x0806;
	};
} // namespace net
