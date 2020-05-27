#pragma once

#include <stdint.h>
#include <stddef.h>
#include <net/mac.hpp>
#include <net/ether.hpp>
#include <net/ipv4.hpp>
#include <net/process.hpp>
#include <async/doorbell.hpp>

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

		static arp_frame query_for(mac from_mac, ipv4_addr from_ip, ipv4_addr for_ip) {
			return arp_frame{0x0001, 0x0800, 6, 4, 1, from_mac, from_ip, {}, for_ip};
		}

		static arp_frame reply_for(mac from_mac, ipv4_addr from_ip, mac for_mac, ipv4_addr for_ip) {
			return arp_frame{0x0001, 0x0800, 6, 4, 2, from_mac, from_ip, for_mac, for_ip};
		}
	};

	// put somewhere else?
	struct route {
		ipv4_addr ip_;
		mac mac_;
		bool resolved_;
		async::doorbell doorbell_;

		bool operator==(const route &other) const {
			return ip_ == other.ip_ && mac_ == other.mac_;
		}

		frg::default_list_hook<route> node_;
	};

	struct arp_processor {
		using from_frame_type = ethernet_frame;

		arp_processor()
		: routes_{}, sender_{nullptr}, our_ip_{} { }

		void attach_sender(sender *s) { sender_ = s; }
		void set_our_ip(ipv4_addr ip) { our_ip_ = ip; }

		async::result<void> push_packet(mem::buffer &&b, ethernet_frame &&);
		bool matches(const ethernet_frame &f);
		async::result<frg::optional<mac>> mac_of(ipv4_addr ip);

	private:
		async::result<void> submit_query_for(ipv4_addr ip);
		async::result<void> submit_reply_for(mac target_mac, ipv4_addr target_ip);

		frg::intrusive_list<
			route,
			frg::locate_member<
				route,
				frg::default_list_hook<route>,
				&route::node_
			>
		> routes_;

		sender *sender_;
		ipv4_addr our_ip_;
	};

	static_assert(net::processor<arp_processor>);
} // namespace net
