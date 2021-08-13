#pragma once

#include <async/recurring-event.hpp>
#include <async/result.hpp>
#include <net/dispatch.hpp>
#include <lib/logger.hpp>
#include <net/ipv4.hpp>
#include <net/arp.hpp>

#include <frg/list.hpp>

namespace net {
	struct udp_frame {
		uint16_t src_port;
		uint16_t dest_port;
		uint16_t length;
		uint16_t csum;

		void *payload;

		ipv4_frame f;

		static udp_frame from_ipv4_frame(ipv4_frame &data) {
			uint8_t *bytes = static_cast<uint8_t *>(data.payload);
			udp_frame f;
			f.src_port = (uint16_t(bytes[0]) << 8) | bytes[1];
			f.dest_port = (uint16_t(bytes[2]) << 8) | bytes[3];
			f.length = (uint16_t(bytes[4]) << 8) | bytes[5]
					- 8; // exclude header
			f.csum = (uint16_t(bytes[6]) << 8) | bytes[7];
			f.f = data;

			f.payload = bytes + 8;
			assert(data.payload_size - 8 >= f.length
				&& "TODO: fragmentation");

			return f;
		}

		size_t get_size() {
			return 8 + length;
		}

		void *to_bytes(void *ptr) {
			uint8_t *buf = static_cast<uint8_t *>(ptr);
			*buf++ = src_port >> 8;
			*buf++ = src_port & 0xFF;

			*buf++ = dest_port >> 8;
			*buf++ = dest_port & 0xFF;

			*buf++ = (8 + length) >> 8;
			*buf++ = (8 + length) & 0xFF;

			// Checksum
			*buf++ = 0;
			*buf++ = 0;

			memcpy(buf, payload, length);
			buf += length;

			return buf;
		}
	};

	struct udp_processor {
		using from_frame_type = ipv4_frame;

		udp_processor()
		: sender_{nullptr}, sockets_{} { }

		void attach_sender(sender *s) { sender_ = s; }

		async::result<void> push_packet(mem::buffer &&b, ipv4_frame &&);
		bool matches(const ipv4_frame &f);

		struct udp_socket {
			friend udp_processor;

			async::result<mem::buffer> recv();
			async::result<void> send(void *buf, size_t size);

			void close();

			ipv4_addr ip() const {
				return ip_;
			}

			uint16_t out_port() const {
				return out_port_;
			}

			uint16_t in_port() const {
				return in_port_;
			}
		private:
			udp_socket(udp_processor *parent, uint16_t in_port,
					uint16_t out_port, ipv4_addr ip, bool listening)
			: parent_{parent}, in_port_{in_port}, out_port_{out_port},
				ip_{ip}, listening_{listening},
				recv_queue_{mem::get_allocator()} { }

			udp_processor *parent_;

			uint16_t in_port_;
			uint16_t out_port_;
			ipv4_addr ip_;

			bool listening_;

			async::recurring_event notify_;

			async::queue<mem::buffer, mem::allocator> recv_queue_;

			frg::default_list_hook<udp_socket> node_;
		};

		async::result<udp_socket *> listen(uint16_t port);
		udp_socket *connect(ipv4_addr ip, uint16_t port);

		void set_arp_processor(arp_processor *ap) {
			arp_processor_ = ap;
		}

	private:
		sender *sender_;

		frg::intrusive_list<
			udp_socket,
			frg::locate_member<
				udp_socket,
				frg::default_list_hook<udp_socket>,
				&udp_socket::node_
			>
		> sockets_;

		arp_processor *arp_processor_;
	};

	static_assert(net::processor<udp_processor>);
} // namespace net
