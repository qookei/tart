#pragma once

#include <stdint.h>

namespace net {
	struct ipv4_addr {
		uint8_t v[4];
		uint8_t operator[](size_t i) const {
			return v[i];
		}

		bool operator==(const ipv4_addr &other) const {
			return v[0] == other[0]
				&& v[1] == other[1]
				&& v[2] == other[2]
				&& v[3] == other[3];
		}

		bool operator!=(const ipv4_addr &other) const {
			return !(*this == other);
		}

		static ipv4_addr from_bytes(const void *data) {
			const uint8_t *bytes = static_cast<const uint8_t *>(data);
			return ipv4_addr{bytes[0], bytes[1], bytes[2], bytes[3]};
		}

		size_t get_size() {
			return 4;
		}

		void *to_bytes(void *ptr) {
			uint8_t *dest = static_cast<uint8_t *>(ptr);
			for (int i = 0; i < 4; i++)
				*dest++ = v[i];

			return dest;
		}
	};

	struct ipv4_frame {
		ipv4_addr source;
		ipv4_addr dest;
		uint8_t protocol;
		uint8_t ttl;
		uint16_t length;
		uint8_t ihl;

		void *payload;
		size_t payload_size;

		static ipv4_frame from_ethernet_frame(ethernet_frame &data) {
			uint8_t *bytes = static_cast<uint8_t *>(data.payload);
			ipv4_frame f;
			f.source = ipv4_addr::from_bytes(bytes + 12);
			f.dest = ipv4_addr::from_bytes(bytes + 16);
			f.ihl = bytes[0] & 0xF;
			f.ttl = bytes[8];
			f.protocol = bytes[9];
			f.length = (uint16_t(bytes[2]) << 8) | bytes[3];
			f.payload = bytes + f.ihl * 4;
			f.payload_size = f.length - f.ihl * 4;

			return f;
		}

		static constexpr uint8_t icmp_proto = 1;
		static constexpr uint8_t igmp_proto = 2;
		static constexpr uint8_t tcp_proto = 6;
		static constexpr uint8_t udp_proto = 17;
		static constexpr uint8_t encap_proto = 41;
		static constexpr uint8_t ospf_proto = 89;
		static constexpr uint8_t sctp_proto = 132;
	};
} // namespace net
